//
// Created by jz on 16-12-24.
//

#include "Decoder.h"
#include <iterator>

namespace JMedia {
    Decoder::Decoder(AVCodecContext *codec_context):
		m_codec_context(codec_context),
		m_frame(NULL)
	{
		m_frame = av_frame_alloc();
    }

    int Decoder::decode(AVPacket *pkt, list<AVFrame *> &decoded_frame_list) {
        int error;
        std::list<AVFrame *> frame_list;

        if (!pkt || !m_codec_context) {
            error = AVERROR_INVALIDDATA;
            goto error_out;
        }
        error = avcodec_send_packet(m_codec_context, pkt);
        if (error < 0) {
            goto error_out;
        }
		while (error >= 0) {
			error = avcodec_receive_frame(m_codec_context, m_frame);
			if (error == AVERROR(EAGAIN) || error == AVERROR_EOF) {
				decoded_frame_list = frame_list;
				return 0;
			}
			if (error < 0) {
				goto error_out;
			}
			AVFrame *newframe = av_frame_alloc();
			av_frame_move_ref(newframe, m_frame);
			frame_list.push_back(newframe);
		}


    error_out:
		m_error.set_error(error);

        while (!frame_list.empty()) {
            AVFrame *node = frame_list.front();
            av_frame_free(&node);
            frame_list.pop_front();
        }
        return error;
    }

    int Decoder::convert_to_pcm(AVFrame *decoded_frame, vector<uint8_t> &pcm) {
        int error = 0;
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
			m_error.set_error(error);
        }
        return error;
    }
}
