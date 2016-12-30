//
// Created by jz on 16-12-18.
//

#ifndef OPEN_MEDIA_FILE_TRANSCODEAAC_H
#define OPEN_MEDIA_FILE_TRANSCODEAAC_H


#include <iostream>

using namespace std;

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include "LibAVInputFileAudio.h"
#include "LibAVOutputFileAudio.h"

class TransCodeAAC {
public:
    explicit TransCodeAAC(const string &in_file, const string &out_file);
    int init();
    int convert();
    string errors() const ;

private:


private:
    LibAVInputFileAudio             m_in_file;
    LibAVOutputFileAudio            m_out_file;


    mutable string                  m_error;
};


#endif //OPEN_MEDIA_FILE_TRANSCODEAAC_H
