//
// Created by jz on 16-12-24.
//

#ifndef DECODE_AUDIO_JMEDIADECODER_H
#define DECODE_AUDIO_JMEDIADECODER_H




#include <list>
#include <string>
#include <vector>

using namespace std;

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>

};

#include <Error.h>

namespace JMedia{

    class Decoder {
    public:
		Decoder() {};
        Decoder(AVCodecContext *codec_context);
        int decode(AVPacket *pkt, list<AVFrame *> &decoded_frame_list);
        int convert_to_pcm(AVFrame *frame, vector<uint8_t> &pcm);
        const char *errors(){
            return m_error.what();
        }
	protected:
		error						m_error;


    private:
        AVCodecContext              *m_codec_context;
		AVFrame						*m_frame;
    };
}

#endif //DECODE_AUDIO_JMEDIADECODER_H
