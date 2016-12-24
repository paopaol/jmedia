//
// Created by jz on 16-12-24.
//

#ifndef DECODE_AUDIO_JMEDIADECODER_H
#define DECODE_AUDIO_JMEDIADECODER_H




#include <list>
#include <string>

using namespace std;

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>

};

namespace JMedia{
    class Decoder {
    public:
        Decoder(AVCodecContext *codec_context);
        int decode(AVPacket *pkt, list<AVFrame *> &decoded_frame_list);
        string errors() const ;

    private:
        AVCodecContext              *m_codec_context;
        mutable string              m_error;
    };
}

#endif //DECODE_AUDIO_JMEDIADECODER_H
