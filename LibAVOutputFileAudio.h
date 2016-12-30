//
// Created by jz on 16-12-17.
//

#ifndef OPEN_MEDIA_FILE_LIBAVOUTPUTAUDIO_H
#define OPEN_MEDIA_FILE_LIBAVOUTPUTAUDIO_H

#include <iostream>

using namespace std;

extern "C"{
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avstring.h>
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>
}

#include "LibAVInputFileAudio.h"

class LibAVOutputFileAudio {
public:
    explicit LibAVOutputFileAudio(const string filename, LibAVInputFileAudio &input_file);
    ~LibAVOutputFileAudio();

    int open();
    string error() const ;
    AVCodecContext  *CodecContext();
    int write_header();
    int write_trailer();
    int init_audio_fifo();
    int init_resample();

private:
    string                      m_filename;
    LibAVInputFileAudio         &m_input_file;
    AVFormatContext             *m_output_format_context;
    AVCodecContext              *m_output_codec_context;
    AVAudioResampleContext      *m_resample_context;
    AVAudioFifo                 *m_out_fifo;
    mutable string              m_error;
    AVFrame                     *m_frame;

};


#endif //OPEN_MEDIA_FILE_LIBAVOUTPUTAUDIO_H
