//
// Created by jz on 16-12-17.
//

#ifndef OPEN_MEDIA_FILE_AVINPUTFILE_H
#define OPEN_MEDIA_FILE_AVINPUTFILE_H

#include <iostream>
#include <string>

using namespace std;

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavresample/avresample.h>

};

#include "LibAVDecoder.h"


struct LibAVPacket{
    LibAVPacket() {
        av_init_packet(&m_pkt);
        m_pkt.data = NULL;
        m_pkt.size = 0;
    }
    ~_LibAVPacket(){
        av_packet_unref(&m_pkt);
    }


    AVPacket    m_pkt;
};





class LibAVInputFileAudio {
public:
    LibAVInputFileAudio(const string &filename);
    ~LibAVInputFileAudio();
    int open();
    int read_packet(LibAVPacket &pkt);
    int read_pcm(string &pcm);
    int convert_to_pcm(AVFrame *frame, string &pcm);
    string &error() const ;
    AVCodecContext  *CodecContext();
private:
    string                      m_filename;
    AVFormatContext             *m_input_format_context;
    AVCodecContext              *m_input_codec_context;
    LibAVDecoder                m_decoder;
    int                         m_stream_index;
    mutable string              m_error;
};


#endif //OPEN_MEDIA_FILE_AVINPUTFILE_H
