//
// Created by jz on 16-12-31.
//

#include "FilterConfig_aformat.h"

extern "C"{
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
}
namespace JMedia{
    FilterConfig_aformat::FilterConfig_aformat(FilterGraph *filter_graph, const std::string &name) {
        m_filter = avfilter_get_by_name("aformat");
        m_filter_ctx = avfilter_graph_alloc_filter(filter_graph->getAVFilterGraph(), m_filter, name.c_str());
        if (!m_filter || !m_filter_ctx) {
            throw Error(AVERROR(ENOMEM));
        }
    }

    int FilterConfig_aformat::set_channel_layout_s(uint64_t channel_layout) {
        char ch_layout[64] = {0};

        av_get_channel_layout_string(ch_layout, sizeof(ch_layout), 0, channel_layout);
        int error = av_opt_set(m_filter_ctx, "channel_layouts", ch_layout, AV_OPT_SEARCH_CHILDREN);
        this->set_error(error);
        return error;
    }

    int FilterConfig_aformat::set_sample_fmt_s(AVSampleFormat sample_fmt) {
        int error = av_opt_set(m_filter_ctx, "sample_fmts", av_get_sample_fmt_name(sample_fmt), AV_OPT_SEARCH_CHILDREN);
        this->set_error(error);
        return error;
    }


    int FilterConfig_aformat::set_sample_rate_s(int sample_rate) {

        char    opt[128] = {0};

        sprintf(opt, "%d", sample_rate);

//        int error = avfilter_init_str(m_filter_ctx, opt);
        int error = av_opt_set(m_filter_ctx, "sample_rates", opt, AV_OPT_SEARCH_CHILDREN);
        this->set_error(error);
        return error;
    }

    int FilterConfig_aformat::init() {
        int error = avfilter_init_str(m_filter_ctx, NULL);
        this->set_error(error);
        return error;
    }
}