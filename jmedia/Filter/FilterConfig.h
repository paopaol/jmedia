//
// Created by jz on 16-12-25.
//

#ifndef DECODE_AUDIO_FILTERCONFIG_H
#define DECODE_AUDIO_FILTERCONFIG_H

#include <string>


extern "C" {
#include <libavfilter/avfilter.h>
#include <libavutil/samplefmt.h>
#include <libavutil/rational.h>
}

#include "../Error.h"



namespace JMedia {
    class FilterConfig {
    public:
        FilterConfig();


        FilterConfig &link(FilterConfig &filter_config);
        AVFilterContext *getAVFilterContext();
    protected:
        int  set_error(int error_no);

    protected:
        AVFilterContext                 *m_filter_ctx;
        AVFilter                        *m_filter;
    private:
        int                             m_error_no;
        mutable std::string             m_error_string;
    };
}

#endif //DECODE_AUDIO_FILTERCONFIG_H
