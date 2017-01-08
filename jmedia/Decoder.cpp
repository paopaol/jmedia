//
// Created by jz on 16-12-24.
//

#include "Decoder.h"
#include <iterator>

namespace JMedia {
    Decoder::Decoder(AVCodecContext *codec_context) {
        m_codec_context = codec_context;
    }

    int Decoder::decode(AVPacket *pkt, list<AVFrame *> &decoded_frame_list) {
        int error;
        char error_str[1024] = {0};
        std::list<AVFrame *> frame_list;
        AVFrame *frame = NULL;

        if (!pkt || !m_codec_context) {
            error = AVERROR_INVALIDDATA;
            goto error_out;
        }

        frame = av_frame_alloc();
        error = avcodec_send_packet(m_codec_context, pkt);
        if (error < 0) {
            goto error_out;
        }
        while (error >= 0) {
            error = avcodec_receive_frame(m_codec_context, frame);
            if (error == AVERROR(EAGAIN) || error == AVERROR_EOF) {
                decoded_frame_list = frame_list;
                av_frame_free(&frame);
                return 0;
            }
            if (error < 0) {
                goto error_out;
            }
            AVFrame *newframe = av_frame_clone(frame);
            av_frame_unref(frame);
            frame_list.push_back(newframe);
        }
    error_out:
        error = AVERROR_INVALIDDATA;
        av_strerror(error, error_str, sizeof(error_str));
        m_error = error_str;

        while (!frame_list.empty()) {
            AVFrame *node = frame_list.front();
            av_frame_free(&node);
            frame_list.pop_front();
        }
        if (frame){
            av_frame_free(&frame);
        }
        return error;
    }

    int Decoder::convert_to_pcm(AVFrame *decoded_frame, vector<uint8_t> &pcm) {
        int error = 0;
        char error_str[1024] = {0};
        int data_size = 0;

        if (!decoded_frame){
            error = AVERROR_INVALIDDATA;
            goto __return;
        }

        pcm.clear();
        data_size = av_get_bytes_per_sample((AVSampleFormat)decoded_frame->format);
        for (int i = 0; i < decoded_frame->nb_samples; i++) {
            if (av_sample_fmt_is_planar((AVSampleFormat)decoded_frame->format)){
                for (int ch = 0; ch < decoded_frame->channels; ch++) {
                    uint8_t *data = decoded_frame->extended_data[ch] + data_size * i;
                    std::copy(data, data + data_size, std::back_inserter(pcm));
                }
            }else{
                uint8_t *data = decoded_frame->extended_data[0] + data_size * i;
                std::copy(data, data + data_size, std::back_inserter(pcm));
            }
        }
        __return:
        if (error < 0) {
            av_strerror(error, error_str, sizeof(error_str));
            m_error = error_str;
        }
        return error;
    }



    string &Decoder::errors() const {
        return m_error;
    }
}
