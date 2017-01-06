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
        Reader();

        const char *errors()const ;

        void set_error(int error_code);

    public:
        virtual ~Reader() = 0;

        virtual int open() = 0;

        virtual int read_packet(Packet &pkt) = 0;

        virtual AVMediaType media_type(Packet &pkt) = 0;

        virtual Decoder &find_decoder(AVMediaType media_type) = 0;

        virtual AVCodecContext *getCodecContext(AVMediaType media_type) = 0;

    protected:
        error               m_error;
    };
}

#endif //DECODE_AUDIO_READER_H
