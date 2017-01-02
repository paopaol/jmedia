//
// Created by jz on 16-12-26.
//

#ifndef DECODE_AUDIO_FILTERCONFIG_ABUFFERSINK_H
#define DECODE_AUDIO_FILTERCONFIG_ABUFFERSINK_H


extern "C" {
#include <libavfilter/avfilter.h>
#include <libavutil/samplefmt.h>
#include <libavutil/rational.h>
}

#include "../Error.h"
#include "FilterConfig.h"
#include "FilterGraph.h"


namespace JMedia {
    class FilterConfig_abuffersink: public FilterConfig{
    public:
        FilterConfig_abuffersink(FilterGraph *filter_graph, const std::string &name);

        int init();
    };
}


#endif //DECODE_AUDIO_FILTERCONFIG_ABUFFERSINK_H
