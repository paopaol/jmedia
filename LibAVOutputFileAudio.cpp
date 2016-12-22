//
// Created by jz on 16-12-17.
//

#include "LibAVOutputFileAudio.h"

extern "C"{
#include <libavutil/opt.h>
}

LibAVOutputFileAudio::LibAVOutputFileAudio(const string filename, LibAVInputFileAudio &input_file)
:m_input_file(input_file)
{
    m_filename = filename;
    m_output_format_context = NULL;
    m_output_codec_context = NULL;
    m_resample_context = NULL;
    m_out_fifo = NULL;
}


LibAVOutputFileAudio::~LibAVOutputFileAudio() {
    if (m_out_fifo){
        av_audio_fifo_free(m_out_fifo);
    }
    if (m_resample_context){
        avresample_close(m_resample_context);
        avresample_free(&m_resample_context);
    }
    if (m_output_codec_context){
        avcodec_free_context(&m_output_codec_context);

    }
    if (m_output_format_context){
        if (m_output_format_context->pb){
            avio_close(m_output_format_context->pb);
        }
        avformat_free_context(m_output_format_context);
    }
}

//核心数据结构就是编解码上下文,承上启下

int LibAVOutputFileAudio::open() {
    int                         error;
    char                        error_str[2048] = {0};
    AVStream                    *stream = NULL;
    AVIOContext                 *output_io_context = NULL;
    AVCodec                     *output_codec = NULL;

    m_output_format_context = avformat_alloc_context();
    if (!m_output_format_context){
        av_strerror(AVERROR(ENOMEM), error_str, sizeof(error_str));
        m_error = error_str;
        error = AVERROR(ENOMEM);
        goto cleanup;
    }
    m_output_format_context->oformat = av_guess_format(NULL, m_filename.c_str(), NULL);
    if (!m_output_format_context->oformat){
        m_error = "Could not find output file format";
        error = AVERROR_EXIT;
        goto cleanup;
    }
    av_strlcpy(m_output_format_context->filename, m_filename.c_str(), sizeof(m_output_format_context->filename));
    error = avio_open(&output_io_context, m_filename.c_str(), AVIO_FLAG_WRITE);
    if (error < 0){
        av_strerror(error, error_str, sizeof(error_str));
        m_error = error_str;
        goto cleanup;
    }
    m_output_format_context->pb = output_io_context;


    output_codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!output_codec){
        m_error = "Could not find an AAC encoder.";
        error = AVERROR_EXIT;
        goto cleanup;
    }
    m_output_codec_context = avcodec_alloc_context3(output_codec);
    if (!m_output_codec_context){
        av_strerror(AVERROR(ENOMEM), error_str, sizeof(error_str));
        m_error = error_str;
        error = AVERROR(ENOMEM);
        goto cleanup;
    }
    m_output_codec_context->channels = 2;
    m_output_codec_context->channel_layout =  av_get_default_channel_layout(2);
    //只有采样率是从输入文件里面获取的
    m_output_codec_context->sample_rate = m_input_file.CodecContext()->sample_rate;
    //位深度,比如aac的codec，可以支持很多位深度，这里使用第一种,下标0
    m_output_codec_context->sample_fmt = output_codec->sample_fmts[0];
    m_output_codec_context->bit_rate = 96000;
    m_output_codec_context->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;


    //判断aac的输出格式，是不是有全局header
    if (m_output_format_context->oformat->flags & AVFMT_GLOBALHEADER){
        m_output_codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    error = avcodec_open2(m_output_codec_context, output_codec, NULL);
    if (error < 0){
        av_strerror(error, error_str, sizeof(error_str));
        m_error = error_str;
        goto cleanup;
    }

    stream = avformat_new_stream(m_output_format_context, NULL);
    if (!stream){
        av_strerror(AVERROR(ENOMEM), error_str, sizeof(error_str));
        m_error = error_str;
        error = AVERROR(ENOMEM);
        goto cleanup;
    }
    stream->time_base.den = m_input_file.CodecContext()->sample_rate;
    stream->time_base.num = 1;

    error = avcodec_parameters_from_context(stream->codecpar, m_output_codec_context);
    if (error < 0){
        av_strerror(error, error_str, sizeof(error_str));
        m_error = error_str;
        goto cleanup;
    }
    return 0;

cleanup:
    if (output_io_context){
        avio_close(output_io_context);
    }
    return error;

}

string LibAVOutputFileAudio::error() const {
    return m_error;
}

AVCodecContext* LibAVOutputFileAudio::CodecContext() {
    return m_output_codec_context;
}

int LibAVOutputFileAudio::write_header() {
    int         error;
    char        error_str[2038] = {0};

    error = avformat_write_header(m_output_format_context, NULL);
    if (error != 0){
        av_strerror(error, error_str, sizeof(error_str));
        m_error = error_str;
        return error;
    }
    return 0;
}

int LibAVOutputFileAudio::write_trailer() {
    int         error;
    char        error_str[2048] = {0};

    error = av_write_trailer(m_output_format_context);
    if (error != 0){
        av_strerror(error, error_str, sizeof(error_str));
        m_error = error_str;
        return error;
    }
    return 0;
}


int LibAVOutputFileAudio::init_audio_fifo() {
    int         error;
    char        error_str[2048] = {0};

    m_out_fifo = av_audio_fifo_alloc(m_output_codec_context->sample_fmt, m_output_codec_context->channels, 1);
    if (!m_out_fifo){
        error = AVERROR(ENOMEM);
        av_strerror(error, error_str, sizeof(error_str));
        m_error = error_str;
        return error;
    }
    return 0;
}
//如果采样参数不一样，就得重新采样，达到这个目的很简单，只要设置好一些重要的参数就行了
int LibAVOutputFileAudio::init_resample() {
    if (m_input_file.CodecContext()->channels != m_output_codec_context->channels ||
            m_input_file.CodecContext()->sample_fmt != m_output_codec_context->sample_fmt){
        int     error;
        char    error_str[2048] = {0};

        m_resample_context = avresample_alloc_context();
        if (!m_resample_context){
            error = AVERROR(ENOMEM);
            av_strerror(error, error_str, sizeof(error_str));
            m_error = error_str;
            return error;
        }
        //通道布局
        av_opt_set_int(m_resample_context, "in_channel_layout", av_get_default_channel_layout(m_input_file.CodecContext()->channels), 0);
        //采样率
        av_opt_set_int(m_resample_context, "in_sample_rate", m_input_file.CodecContext()->sample_rate, 0);
        //位深度
        av_opt_set_int(m_resample_context, "in_sample_fmt", m_input_file.CodecContext()->sample_fmt, 0);

        av_opt_set_int(m_resample_context, "out_channel_layout", av_get_default_channel_layout(m_output_codec_context->channels), 0);
        av_opt_set_int(m_resample_context, "out_sample_rate", m_output_codec_context->sample_rate, 0);
        av_opt_set_int(m_resample_context, "out_sample_fmt", m_output_codec_context->sample_fmt, 0);

        error = avresample_open(m_resample_context);
        if (error != 0){
            av_strerror(error, error_str, sizeof(error_str));
            m_error = error_str;
            return error;
        }
    }
    return 0;
}

