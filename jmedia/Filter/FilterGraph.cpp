//
// Created by jz on 16-12-25.
//

#include "FilterGraph.h"

extern "C"{
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
}


#include "FilterConfig.h"


namespace  JMedia{
    FilterGraph::FilterGraph(){
        m_filter_graph = avfilter_graph_alloc();
        if (!m_filter_graph){
            throw Error(AVERROR(ENOMEM));
        }
        m_src = NULL;
        m_sink = NULL;
    }
    FilterGraph::~FilterGraph() {
        if (m_filter_graph){
            avfilter_graph_free(&m_filter_graph);
        }
    }

    AVFilterGraph * FilterGraph::getAVFilterGraph() {
        return m_filter_graph;
    }

    int FilterGraph::config() {
        int error = avfilter_graph_config(m_filter_graph, NULL);

        return 0;
    }


    int FilterGraph::set_src_sink(FilterConfig &src, FilterConfig &sink) {
        m_src = src.getAVFilterContext();
        m_sink = sink.getAVFilterContext();

        return 0;
    }

    int FilterGraph::src_add_frame(AVFrame *frame) {
        int error = av_buffersrc_add_frame(m_src, frame);
        this->set_error(error);
        return error;
    }

    int FilterGraph::sink_get_frame(AVFrame *frame) {
        int error = av_buffersink_get_frame(m_sink, frame);
        this->set_error(error);
        return error;
    }



    int FilterGraph::set_error(int error) {
        char err_str[1024] = {0};

        av_strerror(error, err_str, sizeof(err_str));
        m_error_string = err_str;
        return 0;
    }

    std::string& FilterGraph::errors() const {
        return m_error_string;
    }

}