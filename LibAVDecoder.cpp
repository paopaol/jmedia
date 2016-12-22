//
// Created by jz on 16-12-21.
//

#include "LibAVDecoder.h"
#include <assert.h>

LibAVDecoder::LibAVDecoder(AVCodecContext *&codec_context):
    m_codec_context(codec_context)
{
    m_frame = av_frame_alloc();
}

LibAVDecoder::~LibAVDecoder() {
    av_frame_free(&m_frame);
}

int LibAVDecoder::decode(AVPacket *pkt, list<AVFrame *> &decoded_frame_list) {
    int                     error;
    char                    error_str[1024] = {0};
    std::list<AVFrame *>    frame_list;

    if (!pkt || !m_codec_context){
        error = AVERROR_INVALIDDATA;
        goto error_out;
    }

    error = avcodec_send_packet(m_codec_context, pkt);
    if (error < 0){
        goto error_out;
    }
    while(error >= 0){
        error = avcodec_receive_frame(m_codec_context, m_frame);
        if (error == AVERROR(EAGAIN) || error == AVERROR_EOF){
            decoded_frame_list = frame_list;
            return 0;
        }
        if (error < 0){
            goto error_out;
        }
        AVFrame *newframe = av_frame_clone(m_frame);
        av_frame_unref(m_frame);
        frame_list.push_back(newframe);
    }
error_out:
    error = AVERROR_INVALIDDATA;
    av_strerror(error, error_str, sizeof(error_str));
    m_error = error_str;

    while(!frame_list.empty()){
        AVFrame *node = frame_list.front();
        av_frame_free(&node);
        frame_list.pop_front();
    }
    return error;
}

string LibAVDecoder::errors() const {
    return m_error;
}