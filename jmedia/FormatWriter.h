//
// Created by jz on 17-1-10.
//

#ifndef DECODE_AUDIO_FORMATWRITER_H
#define DECODE_AUDIO_FORMATWRITER_H

#include <string>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#include "Error.h"

namespace JMedia {
    class FormatWriter {
    public:
        FormatWriter(const std::string &filename);
        ~FormatWriter();

    private:
        AVFormatContext             *m_output_format_context;
        std::string                 m_output_file;
        error                       m_error;
    };
}


#endif //DECODE_AUDIO_FORMATWRITER_H
