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
#include "JMediaDecoder.h"

using namespace std;
using namespace JMedia;

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavresample/avresample.h>

};

namespace JMedia {
    class Error:public exception{
    public:
        Error(int error){
            char    err_str[1024] = {0};

            av_strerror(error, err_str, sizeof(err_str));
            m_error_string = err_str;
        }
        const char *what(){
            return m_error_string.c_str();
        }

    private:
        int     m_error_code;
        string  m_error_string;
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
        Decoder &find_decoder(AVMediaType media_type) throw(Error);
        int read_pcm(string &pcm);
        int convert_to_pcm(AVFrame *frame, string &pcm);
        string &error() const ;
        AVCodecContext  *CodecContext(AVMediaType media_type);
    private:
        string                                      m_filename;
        AVFormatContext                             *m_input_format_context;

        typedef tuple<AVCodecContext *, Decoder, int> Stream;

        map< AVMediaType , Stream>                  m_decoder;
        mutable string                              m_error;
    };
};









#endif //DECODE_AUDIO_JMEDIAREADER_H
