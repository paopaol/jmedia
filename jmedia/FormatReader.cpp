//
// Created by jz on 16-12-24.
//

#include <list>
#include <string>

#include "FormatReader.h"
#include <stdio.h>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

namespace JMedia {

    FormatReader::FormatReader(const std::string &filename)
    {
        m_filename = filename;
        m_input_format_context = NULL;
    }

    FormatReader::~FormatReader() {
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


    int FormatReader::open() {
        int error_code;
        AVCodec *input_codec = NULL;

        error_code = avformat_open_input(&m_input_format_context, m_filename.c_str(), NULL, NULL);
        if (error_code < 0) {
            m_error.set_error(error_code); 
            return error_code;
        }
        error_code = avformat_find_stream_info(m_input_format_context, NULL);
        if (error_code < 0) {
            m_error.set_error(error_code); 
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

            AVCodecContext *codec_context = avcodec_alloc_context3(input_codec);
            if (!codec_context) {
                m_error.set_error(AVERROR(ENOMEM)); 
                return AVERROR(ENOMEM);
            }
            error_code = avcodec_parameters_to_context(codec_context, codecpar);
            if (error_code < 0) {
                m_error.set_error(error_code); 
                return error_code;
            }
            error_code = avcodec_open2(codec_context, input_codec, NULL);
            if (error_code < 0) {
                m_error.set_error(error_code); 
                return error_code;
            }
            Decoder decoder = Decoder(codec_context);
            Stream stream = {media_type,codec_context, decoder, stream_index};
            m_streams.push_back(stream);
        }
        return 0;
    }


    int FormatReader::read_packet(Packet &pkt) {
        int error = 0;
        char error_str[1024] = {0};

        if (m_input_format_context == nullptr){
            error = AVERROR_INVALIDDATA;
            goto __return;
        }

        error = av_read_frame(m_input_format_context, &pkt.m_pkt);
        if (error < 0) {
            goto __return;
        }

        __return:
        if (error < 0) {
            m_error.set_error(error); 
        }
        return error;
    }


    AVMediaType FormatReader::media_type(Packet &pkt) {
        int stream_index = pkt.m_pkt.stream_index;

        for (auto it = m_streams.begin(); it != m_streams.end(); it++){
            if (it->stream_index == stream_index){
                return it->media_type;
            }
        }

        return AVMEDIA_TYPE_UNKNOWN;
    }

    Decoder &FormatReader::find_decoder(AVMediaType media_type) {
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


