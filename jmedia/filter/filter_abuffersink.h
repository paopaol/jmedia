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

#include <Error.h>
#include "filter.h"
#include "filter_graph.h"


namespace JMedia {
    class FilterAbuffersink: public Filter{
    public:
        FilterAbuffersink(FilterGraph *filter_graph, const std::string &name);

        int init();
    };
}


#endif //DECODE_AUDIO_FILTERCONFIG_ABUFFERSINK_H
