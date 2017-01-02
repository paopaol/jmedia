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
        Packet() {
            av_init_packet(&m_pkt);
            m_pkt.data = NULL;
            m_pkt.size = 0;
        }
        ~Packet(){
            av_packet_unref(&m_pkt);
        }

        AVPacket    m_pkt;
    };
}

#endif //DECODE_AUDIO_BASE_H
