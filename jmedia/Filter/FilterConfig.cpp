//
// Created by jz on 16-12-26.
//

#include "FilterConfig.h"

namespace JMedia{
    FilterConfig::FilterConfig(){
        m_filter = NULL;
        m_filter_ctx = NULL;
        m_error_no = 0;
    };

    int FilterConfig::set_error(int error_no){
        char err_str[1024] = {0};

        if (error_no < 0) {
            av_strerror(error_no, err_str, sizeof(err_str));
            m_error_string = err_str;
            m_error_no = error_no;
        }

        return 0;
    }

    FilterConfig& FilterConfig::link(FilterConfig &filter_config) throw(Error){
        int error = avfilter_link(m_filter_ctx, 0, filter_config.m_filter_ctx,0);
        if (error < 0){
            throw Error(error);
        }
    }

    AVFilterContext* FilterConfig::getAVFilterContext() {
        return m_filter_ctx;
    }
}