//
// Created by jz on 16-12-17.
//

#include "LibAVInputFileAudio.h"

#include <string>


LibAVInputFileAudio::LibAVInputFileAudio(const string &filename)
        :m_input_codec_context(NULL),
        m_decoder(m_input_codec_context)
{
    m_filename = filename;
    m_input_format_context = NULL;
}

LibAVInputFileAudio::~LibAVInputFileAudio() {
    if (m_input_codec_context){
        avcodec_free_context(&m_input_codec_context);
    }
    if (m_input_format_context){
        avformat_close_input(&m_input_format_context);
    }
}


int LibAVInputFileAudio::open() {
    int                 error_code;
    char                error_str[2048] = {0};
    AVCodec             *input_codec = NULL;

    //打开文件流
    error_code = avformat_open_input(&m_input_format_context, m_filename.c_str(), NULL, NULL);
    if (error_code < 0){
        av_strerror(error_code, error_str, sizeof(error_str));
        m_error = error_str;
        return error_code;
    }
    //查找流信息
    error_code = avformat_find_stream_info(m_input_format_context, NULL);
    if (error_code < 0){
        av_strerror(error_code, error_str, sizeof(error_str));
        m_error = error_str;
        return error_code;
    }
    for (int i = 0; i < m_input_format_context->nb_streams; i++){
        if (m_input_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            //查找decoder
            input_codec = avcodec_find_decoder(m_input_format_context->streams[0]->codecpar->codec_id);
            if (!input_codec){
                break;
            }
            m_stream_index = i;
        }
    }
    if (!input_codec){
        snprintf(error_str, sizeof(error_str), "Could not find input codec");
        m_error = error_str;
        return AVERROR_EXIT;
    }

    //利用decoder初始化codec上下文
    m_input_codec_context = avcodec_alloc_context3(input_codec);
    if (!m_input_codec_context){
        snprintf(error_str, sizeof(error_str), "Could not allocate a decoding context");
        m_error = error_str;
        return AVERROR_EXIT;
    }
    //将参数填充到codec上下文
    error_code = avcodec_parameters_to_context(m_input_codec_context, m_input_format_context->streams[0]->codecpar);
    if (error_code < 0){
        av_strerror(error_code, error_str, sizeof(error_str));
        m_error = error_str;
        return error_code;
    }
    //打开codec
    error_code = avcodec_open2(m_input_codec_context, input_codec, NULL);
    if (error_code < 0){
        av_strerror(error_code, error_str, sizeof(error_str));
        m_error = error_str;
        return error_code;
    }
    return 0;
}



int LibAVInputFileAudio::read_packet(LibAVPacket &pkt) {
    int                     error = 0;
    char                    error_str[1024] = {0};

    error = av_read_frame(m_input_format_context, &pkt.m_pkt);
    if (error < 0) {
        goto __return;
    }

__return:
    if (error < 0){
        av_strerror(error, error_str, sizeof(error_str));
        m_error = error_str;
    }
    return error;
}


int LibAVInputFileAudio::convert_to_pcm(AVFrame *frame, string &pcm) {
    int                     error = 0;
    char                    error_str[1024] = {0};

    if (!frame){
        error = AVERROR_INVALIDDATA;
        goto __return;
    }
    pcm.resize(0, 0);
    int data_size = av_get_bytes_per_sample(m_input_codec_context->sample_fmt);
    for (int i = 0; i < frame->nb_samples; i++){
        for (int ch = 0; ch < m_input_codec_context->channels; ch++){
            pcm.append((char *)frame->data[ch] + data_size * i, data_size);
        }
    }
__return:
    if (error < 0){
        av_strerror(error, error_str, sizeof(error_str));
        m_error = error_str;
    }
    return error;
}


int LibAVInputFileAudio::read_pcm(string &pcm) {
    int                     error = 0;
    char                    error_str[1024] = {0};
    LibAVPacket            pkt;
    int                     got_frame = 0;
    std::list<AVFrame *>    list;

    while (1) {
        error = av_read_frame(m_input_format_context, &pkt.m_pkt);
        if (error < 0) {
            goto __return;
        }

        if (pkt.m_pkt.stream_index == m_stream_index){
            error = m_decoder.decode(&pkt.m_pkt, list);
            if (error < 0){
                goto __return;
            }
            int data_size = av_get_bytes_per_sample(m_input_codec_context->sample_fmt);
            for (auto dframe = list.begin(); dframe != list.end(); dframe++){
                AVFrame *f = *dframe;
                for (int i = 0; i < f->nb_samples; i++){
                    for (int ch = 0; ch < m_input_codec_context->channels; ch++){
                        pcm.append((char *)f->data[ch] + data_size * i, data_size);
                    }
                }
            }
            error = 0;
            goto __return;
        }
    }
__return:
    while (!list.empty()){
        AVFrame *f = list.front();
        av_frame_free(&f);
        list.pop_front();
    }
    if (error < 0){
        av_strerror(error, error_str, sizeof(error_str));
        m_error = error_str;
    }
    return error;
}

string &LibAVInputFileAudio::error() const {
    return m_error;
}

AVCodecContext *LibAVInputFileAudio::CodecContext() {
    return m_input_codec_context;
}

