//
// Created by jz on 16-12-24.
//

#include <list>
#include "Reader.h"
#include <stdio.h>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

namespace JMedia {

    Reader::Reader(const string &filename)
    {
        m_filename = filename;
        m_input_format_context = NULL;
    }

    Reader::~Reader() {
        for (auto stream = m_streams.begin(); stream != m_streams.end(); stream++){
            AVCodecContext  *codec_context = stream->codec_context;
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
        for (unsigned int i = 0; i < m_input_format_context->nb_streams; i++) {
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
                char    error_str[1024] = {0};
                av_strerror(AVERROR(ENOMEM), error_str, sizeof(error_str));
                m_error = error_str;
                return AVERROR(ENOMEM);
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
            Stream stream = {media_type,codec_context, decoder, stream_index};
            m_streams.push_back(stream);
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

        for (auto it = m_streams.begin(); it != m_streams.end(); it++){
            if (it->stream_index == stream_index){
                return it->media_type;
            }
        }

        return AVMEDIA_TYPE_UNKNOWN;
    }

    Decoder &Reader::find_decoder(AVMediaType media_type) {
        for (auto it = m_streams.begin(); it != m_streams.end(); it++){
            if (it->media_type == media_type){
                return it->decoder;
            }
        }
        throw Error(AVERROR_DECODER_NOT_FOUND);
    }





    string &Reader::error() const {
        return m_error;
    }

    AVCodecContext *Reader::CodecContext(AVMediaType media_type) {
        for (auto it = m_streams.begin(); it != m_streams.end(); it++){
            if (it->media_type == media_type){
                return it->codec_context;
            }
        }
        return NULL;
    }

}


