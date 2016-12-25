//
// Created by jz on 16-12-25.
//

#ifndef DECODE_AUDIO_ERROR_H
#define DECODE_AUDIO_ERROR_H

#include <exception>
#include <string>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavresample/avresample.h>

};

namespace JMedia {
    class Error : public std::exception {
    public:
        Error(int error) {
            char err_str[1024] = {0};

            av_strerror(error, err_str, sizeof(err_str));
            m_error_string = err_str;
        }


        const char *what() {
            return m_error_string.c_str();
        }

    private:
        int         m_error_code;
        std::string m_error_string;
    };
}

#endif //DECODE_AUDIO_ERROR_H
