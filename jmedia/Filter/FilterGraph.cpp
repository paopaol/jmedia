//
// Created by jz on 16-12-25.
//

#include "FilterGraph.h"

extern "C"{
#include <libavfilter/buffersrc.h>
}

namespace  JMedia{
    FilterGraph::FilterGraph() throw(Error){
        m_filter_graph = avfilter_graph_alloc();
        if (!m_filter_graph){
            throw Error(AVERROR(ENOMEM));
        }
    }
    FilterGraph::~FilterGraph() {
        if (m_filter_graph){
            avfilter_graph_free(&m_filter_graph);
        }
    }

    AVFilterGraph * FilterGraph::getAVFilterGraph() {
        return m_filter_graph;
    }







}