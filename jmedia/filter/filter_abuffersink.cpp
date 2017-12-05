//
// Created by jz on 16-12-26.
//

#include "filter_abuffersink.h"

extern "C"{
#include <libavfilter/buffersrc.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
}

namespace JMedia{
    FilterAbuffersink::FilterAbuffersink(FilterGraph *filter_graph, const std::string &name){
        m_filter = avfilter_get_by_name("abuffersink");
        m_filter_ctx = avfilter_graph_alloc_filter(filter_graph->getAVFilterGraph(), m_filter, name.c_str());
        if (!m_filter || !m_filter_ctx) {
            throw Error(AVERROR(ENOMEM));
        }
    }

    int FilterAbuffersink::init() {
        int error =  avfilter_init_str(m_filter_ctx, NULL);
        this->set_error(error);

        return error;
    }
}