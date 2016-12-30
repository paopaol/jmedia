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
#include "FilterConfig.h"

namespace JMedia{
    class FilterGraph {
    public:
        FilterGraph();
        ~FilterGraph();
        AVFilterGraph *getAVFilterGraph();
        int config();
        int set_src_sink(FilterConfig &src, FilterConfig &sink);
        int src_add_frame(AVFrame *frame);
        int sink_get_frame(AVFrame *frame);
        std::string &errors() const ;

    protected:
        int set_error(int error);

    private:
        AVFilterGraph           *m_filter_graph;
        AVFilterContext         *m_src;
        AVFilterContext         *m_sink;

        int                     m_error_no;
        mutable std::string     m_error_string;
    };

}


#endif //DECODE_AUDIO_FILTERGRAPH_H
