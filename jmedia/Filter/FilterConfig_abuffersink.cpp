//
// Created by jz on 16-12-26.
//

#include "FilterConfig_abuffersink.h"

extern "C"{
#include <libavfilter/buffersrc.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
}

namespace JMedia{
    FilterConfig_abuffersink::FilterConfig_abuffersink(FilterGraph *filter_graph, const std::string &name){
        m_filter = avfilter_get_by_name("abuffersink");
        m_filter_ctx = avfilter_graph_alloc_filter(filter_graph->getAVFilterGraph(), m_filter, name.c_str());
        if (!m_filter || !m_filter_ctx) {
            throw Error(AVERROR(ENOMEM));
        }
    }

    int FilterConfig_abuffersink::set_channel_layout(uint64_t channel_layout) {
        int error = av_opt_set_bin(m_filter_ctx, "channel_layouts", (uint8_t *)&channel_layout, sizeof(channel_layout), AV_OPT_SEARCH_CHILDREN);
        this->set_error(error);
        return error;
    }

    int FilterConfig_abuffersink::set_sample_fmt(AVSampleFormat sample_fmt) {
        int error = av_opt_set_bin(m_filter_ctx, "sample_fmts", (uint8_t *)&sample_fmt, sizeof(sample_fmt), AV_OPT_SEARCH_CHILDREN);
        this->set_error(error);
        return error;
    }


    int FilterConfig_abuffersink::set_sample_rate(int sample_rate) {
        int error = av_opt_set_bin(m_filter_ctx, "sample_rates", (uint8_t *)&sample_rate, sizeof(sample_rate), AV_OPT_SEARCH_CHILDREN);
        this->set_error(error);
        return error;
    }


    int FilterConfig_abuffersink::init() {
        int error =  avfilter_init_str(m_filter_ctx, NULL);
        this->set_error(error);

        return error;
    }
}