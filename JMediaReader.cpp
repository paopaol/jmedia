//
// Created by jz on 16-12-24.
//


#include <list>

#include "JMediaReader.h"

namespace JMedia {

    Reader::Reader(const string &filename)
    {
        m_filename = filename;
        m_input_format_context = NULL;
    }

    Reader::~Reader() {
        for (auto decoder = m_decoder.begin(); decoder != m_decoder.end(); decoder++){
            AVCodecContext  *codec_context = std::get<0>(decoder->second);
            if (codec_context){
                avcodec_free_context(&codec_context);
            }
        }
        if (m_input_format_context) {
            avformat_close_input(&m_input_format_context);
        }
    }


    int Reader::open() {
        int error_code;
        char error_str[2048] = {0};
        AVCodec *input_codec = NULL;

        //打开文件流
        error_code = avformat_open_input(&m_input_format_context, m_filename.c_str(), NULL, NULL);
        if (error_code < 0) {
            av_strerror(error_code, error_str, sizeof(error_str));
            m_error = error_str;
            return error_code;
        }
        //查找流信息
        error_code = avformat_find_stream_info(m_input_format_context, NULL);
        if (error_code < 0) {
            av_strerror(error_code, error_str, sizeof(error_str));
            m_error = error_str;
            return error_code;
        }
        for (int i = 0; i < m_input_format_context->nb_streams; i++) {
            AVMediaType media_type = m_input_format_context->streams[i]->codecpar->codec_type;
            AVCodecID codec_id = m_input_format_context->streams[i]->codecpar->codec_id;
            AVCodecParameters *codecpar = m_input_format_context->streams[i]->codecpar;
            int stream_index = i;

            input_codec = avcodec_find_decoder(codec_id);
            if (!input_codec) {
                continue;
            }

            //利用decoder初始化codec上下文
            AVCodecContext *codec_context = avcodec_alloc_context3(input_codec);
            if (!codec_context) {
                snprintf(error_str, sizeof(error_str), "Could not allocate a decoding context");
                m_error = error_str;
                return AVERROR_EXIT;
            }
            //将参数填充到codec上下文
            error_code = avcodec_parameters_to_context(codec_context, codecpar);
            if (error_code < 0) {
                av_strerror(error_code, error_str, sizeof(error_str));
                m_error = error_str;
                return error_code;
            }
            //打开codec
            error_code = avcodec_open2(codec_context, input_codec, NULL);
            if (error_code < 0) {
                av_strerror(error_code, error_str, sizeof(error_str));
                m_error = error_str;
                return error_code;
            }
            Decoder decoder = Decoder(codec_context);
            Stream stream = std::make_tuple(codec_context, decoder, i);
            m_decoder.insert(std::map<AVMediaType, Stream>::value_type(media_type, stream));
        }
        return 0;
    }


    int Reader::read_packet(Packet &pkt) {
        int error = 0;
        char error_str[1024] = {0};

        error = av_read_frame(m_input_format_context, &pkt.m_pkt);
        if (error < 0) {
            goto __return;
        }

        __return:
        if (error < 0) {
            av_strerror(error, error_str, sizeof(error_str));
            m_error = error_str;
        }
        return error;
    }


    AVMediaType Reader::media_type(Packet &pkt) {
        int stream_index = pkt.m_pkt.stream_index;

        for (auto it = m_decoder.begin(); it != m_decoder.end(); it++){
            AVMediaType media_type = it->first;
            Stream stream = it->second;

            if (std::get<2>(stream) == stream_index){
                return media_type;
            }
        }

        return AVMEDIA_TYPE_UNKNOWN;
    }

    Decoder &Reader::find_decoder(AVMediaType media_type) throw(Error) {
        auto it = m_decoder.find(media_type);
        if (it != m_decoder.end()){
            Stream stream = it->second;
            return std::get<1>(stream);
        }
        throw Error(AVERROR_DECODER_NOT_FOUND);
    }



    int Reader::convert_to_pcm(AVFrame *decoded_frame, string &pcm) {
        int error = 0;
        char error_str[1024] = {0};
        AVCodecContext  *codec_context = NULL;
        int data_size = 0;


        auto it = m_decoder.find(AVMEDIA_TYPE_AUDIO);

        if (it == m_decoder.end()){
            error = AVERROR_INVALIDDATA;
            goto __return;
        }

        if (!decoded_frame) {
            error = AVERROR_INVALIDDATA;
            goto __return;
        }

        codec_context = std::get<0>(it->second);
        pcm.resize(0, 0);
        data_size = av_get_bytes_per_sample(codec_context->sample_fmt);
        for (int i = 0; i < decoded_frame->nb_samples; i++) {
            for (int ch = 0; ch < codec_context->channels; ch++) {
                pcm.append((char *) decoded_frame->data[ch] + data_size * i, data_size);
            }
        }
    __return:
        if (error < 0) {
            av_strerror(error, error_str, sizeof(error_str));
            m_error = error_str;
        }
        return error;
    }


    string &Reader::error() const {
        return m_error;
    }

    AVCodecContext *Reader::CodecContext(AVMediaType media_type) {
        auto it = m_decoder.find(media_type);

        if (it == m_decoder.end()){
            return NULL;
        }

        AVCodecContext  *codec_context = std::get<0>(it->second);

        return codec_context;
    }

}


