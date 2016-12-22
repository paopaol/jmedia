//
// Created by jz on 16-12-18.
//

#include "TransCodeAAC.h"
extern "C" {
#include <libavutil/opt.h>
}

TransCodeAAC::TransCodeAAC(const string &in_file, const string &out_file)
:m_in_file(in_file),
 m_out_file(out_file, m_in_file)
{
}


int TransCodeAAC::init() {
    int     error;

    error = m_in_file.open();
    if (error != 0){
        m_error = m_in_file.error();
        return error;
    }
    error = m_out_file.open();
    if (error != 0){
        m_error = m_out_file.error();
        return error;
    }
    error = m_out_file.init_resample();
    if (error != 0){
        m_error = m_out_file.error();
        return error;
    }
    error = m_out_file.init_audio_fifo();
    if (error != 0){
        m_error = m_out_file.error();
        return error;
    }
    error = m_out_file.write_header();
    if (error != 0){
        m_error = m_out_file.error();
        return error;
    }
    return 0;
}

int TransCodeAAC::convert() {
    int             error = 0;
    char            error_str[1024] = {0};
    AVFrame         *frame = NULL;

    frame = av_frame_alloc();
    if (!frame){
        error = AVERROR(ENOMEM);
        goto __return;
    }

    while (1){
        error = m_in_file.read_decoded_frame(frame);
        if (error < 0){
            m_error = m_in_file.error();
            goto __return;
        }

    }
__return:
    if (frame){
        av_frame_free(&frame);
    }

    av_strerror(error, error_str, sizeof(error_str));
    m_error = error_str;
    return error;
}



string TransCodeAAC::errors() const {
    return m_error;
}