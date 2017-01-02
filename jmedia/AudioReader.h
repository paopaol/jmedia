//
// Created by jz on 17-1-2.
//

#ifndef DECODE_AUDIO_AUDIOREADER_H
#define DECODE_AUDIO_AUDIOREADER_H


#include <string>

using namespace std;

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#include "base.h"

class AudioReader {

private:
    mutable
};


#endif //DECODE_AUDIO_AUDIOREADER_H
