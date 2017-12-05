//
// Created by jz on 17-1-2.
//

#ifndef DECODE_AUDIO_READER_H
#define DECODE_AUDIO_READER_H

#include <string>

using namespace std;

#include "base.h"
#include "Decoder.h"
#include "Error.h"

namespace JMedia{
    class Reader{
    public:
        virtual int open() = 0;
        virtual int close() = 0;
        virtual int read_packet(Packet &pkt) = 0;
        virtual AVMediaType media_type(Packet &pkt) = 0;
        virtual int find_decoder(AVMediaType media_type, Decoder &decoder) = 0;
        virtual int getCodecContext(AVMediaType media_type, AVCodecContext *&codecContext) = 0;
        const char *errors(){
            return m_error.what();
        }
    protected:
        error               m_error;
    };
}

#endif //DECODE_AUDIO_READER_H
