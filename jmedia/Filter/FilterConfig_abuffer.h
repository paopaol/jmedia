//
// Created by jz on 16-12-25.
//

#ifndef DECODE_AUDIO_FILTERCONFIG_ABUFFER_H
#define DECODE_AUDIO_FILTERCONFIG_ABUFFER_H

extern "C" {
#include <libavfilter/avfilter.h>
#include <libavutil/samplefmt.h>
#include <libavutil/rational.h>
}

#include "../Error.h"
#include "FilterConfig.h"
#include "FilterGraph.h"


namespace JMedia {
    class FilterConfig_abuffer : public FilterConfig {
    public:
        FilterConfig_abuffer(FilterGraph *filter_graph, const std::string &name)throw(Error);

        int set_channel_layout(int channel_layout);

        int set_sample_fmt(AVSampleFormat sample_fmt);

        int set_time_base(AVRational time_base);

        int set_sample_rate(int sample_rate);

        int init();
    };

}

#endif //DECODE_AUDIO_FILTERCONFIG_ABUFFER_H
