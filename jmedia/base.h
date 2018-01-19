//
// Created by jz on 17-1-2.
//

#ifndef DECODE_AUDIO_BASE_H
#define DECODE_AUDIO_BASE_H

extern "C"{
#include <libavcodec/avcodec.h>
};

namespace JMedia{
    struct Packet{
		void init() {
            av_init_packet(&m_pkt);
            m_pkt.data = NULL;
            m_pkt.size = 0;
		}
		void unref() {
            av_packet_unref(&m_pkt);
		}
		~Packet() {
			unref();
		}
        AVPacket    m_pkt;
    };

    struct Stream{
        AVMediaType         media_type;
        AVCodecContext      *codec_context;
        Decoder             decoder;
        int                 stream_index;
        AVStream			*stream;
    };
}

#endif //DECODE_AUDIO_BASE_H
