//
// Created by jz on 16-12-21.
//

#ifndef DECODE_AUDIO_LIBDECODER_H
#define DECODE_AUDIO_LIBDECODER_H

#include <list>
#include <string>

using namespace std;

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>

};

class LibAVDecoder {
public:
    LibAVDecoder(AVCodecContext *&codec_context);
    ~LibAVDecoder();
    int decode(AVPacket *pkt, list<AVFrame *> &decoded_frame_list);
    string errors() const ;

private:
    AVCodecContext              *&m_codec_context;
    AVFrame                     *m_frame;
    mutable string              m_error;
};


#endif //DECODE_AUDIO_LIBDECODER_H
