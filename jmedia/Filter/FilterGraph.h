//
// Created by jz on 16-12-25.
//

#ifndef DECODE_AUDIO_FILTERGRAPH_H
#define DECODE_AUDIO_FILTERGRAPH_H

#include <string>


extern "C" {
#include <libavfilter/avfilter.h>
#include <libavutil/samplefmt.h>
#include <libavutil/rational.h>
}

#include "../Error.h"

namespace JMedia{
    class FilterGraph {
    public:
        FilterGraph()throw(Error);
        ~FilterGraph();
        AVFilterGraph *getAVFilterGraph();

    private:
        AVFilterGraph       *m_filter_graph;
    };

}


#endif //DECODE_AUDIO_FILTERGRAPH_H
