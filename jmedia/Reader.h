//
// Created by jz on 16-12-24.
//

#ifndef DECODE_AUDIO_JMEDIAREADER_H
#define DECODE_AUDIO_JMEDIAREADER_H

#include <iostream>
#include <exception>
#include <string>
#include <tuple>
#include <map>
#include "Decoder.h"
#include "Error.h"

using namespace std;
using namespace JMedia;

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

namespace JMedia {
    struct Stream{
        AVMediaType         media_type;
        AVCodecContext      *codec_context;
        Decoder             decoder;
        int                 stream_index;
    };


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

    class Reader {
    public:
        Reader(const string &filename);
        ~Reader();
        int open();
        int read_packet(Packet &pkt);
        AVMediaType media_type(Packet &pkt);
        Decoder &find_decoder(AVMediaType media_type);
        string &error() const ;
        AVCodecContext  *CodecContext(AVMediaType media_type);
    private:
        string                                      m_filename;
        AVFormatContext                             *m_input_format_context;

        std::list<Stream>                           m_streams;
        mutable string                              m_error;
    };
};









#endif //DECODE_AUDIO_JMEDIAREADER_H
