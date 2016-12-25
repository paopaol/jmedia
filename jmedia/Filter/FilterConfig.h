//
// Created by jz on 16-12-25.
//

#ifndef DECODE_AUDIO_FILTERCONFIG_H
#define DECODE_AUDIO_FILTERCONFIG_H

extern "C" {
#include <libavfilter/avfilter.h>
#include <libavutil/samplefmt.h>
#include <libavutil/rational.h>
}


namespace JMedia {
    class FilterConfig {
    public:
        FilterConfig(){
            m_filter = NULL;
            m_filter_ctx = NULL;
        };

    protected:
        AVFilterContext *m_filter_ctx;
        AVFilter *m_filter;
    };
}

#endif //DECODE_AUDIO_FILTERCONFIG_H
