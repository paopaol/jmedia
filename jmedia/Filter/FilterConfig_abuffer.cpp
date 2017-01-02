//
// Created by jz on 16-12-25.
//

#include "FilterConfig_abuffer.h"

extern "C"{
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
}

#include "FilterConfig.h"

namespace JMedia {
    FilterConfig_abuffer::FilterConfig_abuffer(FilterGraph *filter_graph, const std::string &name) {
        m_filter = avfilter_get_by_name("abuffer");
        m_filter_ctx = avfilter_graph_alloc_filter(filter_graph->getAVFilterGraph(), m_filter, name.c_str());
        if (!m_filter || !m_filter_ctx) {
            throw Error(AVERROR(ENOMEM));
        }
    }

    int FilterConfig_abuffer::set_channel_layout(uint64_t channel_layout) {
        char ch_layout[64] = {0};

        av_get_channel_layout_string(ch_layout, sizeof(ch_layout), 0, channel_layout);
        int error = av_opt_set(m_filter_ctx, "channel_layout", ch_layout, AV_OPT_SEARCH_CHILDREN);
        this->set_error(error);
        return error;
    }

    int FilterConfig_abuffer::set_sample_fmt(AVSampleFormat sample_fmt) {
        int error = av_opt_set(m_filter_ctx, "sample_fmt", av_get_sample_fmt_name(sample_fmt), AV_OPT_SEARCH_CHILDREN);
        this->set_error(error);
        return error;
    }

    int FilterConfig_abuffer::set_time_base(AVRational time_base) {
        int error = av_opt_set_q(m_filter_ctx, "time_base", time_base, AV_OPT_SEARCH_CHILDREN);
        this->set_error(error);
        return error;
    }

    int FilterConfig_abuffer::set_sample_rate(int sample_rate) {
        int error = av_opt_set_int(m_filter_ctx, "sample_rate", sample_rate, AV_OPT_SEARCH_CHILDREN);
        this->set_error(error);
        return error;
    }

    int FilterConfig_abuffer::init() {
        int error = avfilter_init_str(m_filter_ctx, NULL);
        this->set_error(error);
        return error;
    }
}
