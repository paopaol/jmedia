//
// Created by jz on 16-12-25.
//

#ifndef DECODE_AUDIO_ERROR_H
#define DECODE_AUDIO_ERROR_H

#include <exception>
#include <string>

extern "C"{
#include <libavutil/avutil.h>
};

namespace JMedia {
    class Error{
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

    class error{
    public:
        error(){
            m_error_code = 0;
        }

        const char *what() {
            return m_error_string.c_str();
        }
        void set_error(int error){
            char err_str[1024] = {0};

            av_strerror(error, err_str, sizeof(err_str));
            m_error_string = err_str;
        }
        int error_code(){
            return m_error_code;
        }
    private:
        int         m_error_code;
        std::string m_error_string;
    };
}

#endif //DECODE_AUDIO_ERROR_H
