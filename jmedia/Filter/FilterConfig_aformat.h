//
// Created by jz on 16-12-31.
//

#ifndef DECODE_AUDIO_FILTERCONFIG_AFORMAT_H
#define DECODE_AUDIO_FILTERCONFIG_AFORMAT_H


#include "FilterConfig.h"
#include "FilterGraph.h"

namespace JMedia {
    class FilterConfig_aformat : public FilterConfig {
        public:
            FilterConfig_aformat(FilterGraph *filter_graph, const std::string &name);

            int set_channel_layout_s(uint64_t channel_layout);

            int set_sample_fmt_s(AVSampleFormat sample_fmt);

            int set_sample_rate_s(int sample_rate);

            int init();
    };
}


#endif //DECODE_AUDIO_FILTERCONFIG_AFORMAT_H
