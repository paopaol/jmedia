//
// Created by jz on 16-12-25.
//

#include "filter.h"
#include "filter_graph.h"

extern "C"{
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
}

#include <functional>
#include <memory>
#include <list>
#include <assert.h>



namespace  JMedia{
    FilterGraph::FilterGraph():
		m_filter_graph(NULL),
		m_src(NULL),
		m_sink(NULL),
		m_frame(NULL)
	{
        m_filter_graph = avfilter_graph_alloc();
		m_frame = av_frame_alloc();
    }
    FilterGraph::~FilterGraph() {
		if (m_frame) {
			av_frame_free(&m_frame);
		}
        if (m_filter_graph){
            avfilter_graph_free(&m_filter_graph);
        }
    }

    AVFilterGraph * FilterGraph::getAVFilterGraph() {
        return m_filter_graph;
    }

    int FilterGraph::config(const std::string &filters_descr) {
		assert(m_filter_graph != NULL);

		AVFilterInOut *outputs = avfilter_inout_alloc();

		if (!outputs) {
			m_error.set_error(AVERROR(ENOMEM));
			return AVERROR(ENOMEM);
		}
		std::shared_ptr<void> outfree(nullptr, std::bind(avfilter_inout_free, &outputs));


		AVFilterInOut *inputs = avfilter_inout_alloc();

		if (!inputs) {
			m_error.set_error(AVERROR(ENOMEM));
			return AVERROR(ENOMEM);
		}
		std::shared_ptr<void> infree(nullptr, std::bind(avfilter_inout_free, &inputs));

		outputs->name = av_strdup(m_src->name);
		outputs->filter_ctx = m_src;
		outputs->pad_idx = 0;
		outputs->next = NULL;

		inputs->name = av_strdup(m_sink->name);
		inputs->filter_ctx = m_sink;
		inputs->pad_idx = 0;
		inputs->next = NULL;

		int error = avfilter_graph_parse_ptr(m_filter_graph, filters_descr.c_str(),
			&inputs, &outputs, NULL);
		if (error < 0) {
			m_error.set_error(error);
			return error;
		}

        error = avfilter_graph_config(m_filter_graph, NULL);
		if (error < 0) {
			m_error.set_error(error);
			return error;
		}

        return 0;
    }


    int FilterGraph::set_src_sink(Filter &src, Filter &sink) {
        m_src = src.getAVFilterContext();
        m_sink = sink.getAVFilterContext();

        return 0;
    }

    int FilterGraph::src_add_frame(AVFrame *frame) {
        int error = av_buffersrc_add_frame_flags(m_src, frame, AV_BUFFERSRC_FLAG_KEEP_REF);
        //int error = av_buffersrc_add_frame(m_src, frame);
        m_error.set_error(error);
        return error;
    }

    int FilterGraph::sink_get_frame(std::list<AVFrame *> &filteredFrameList) {
        int error;
		std::list<AVFrame *> gotList;

		while (1) {
			error = av_buffersink_get_frame(m_sink, m_frame);
			if (error == AVERROR(EAGAIN) || error == AVERROR_EOF) {
				filteredFrameList = gotList;
				return 0;
			}
			if (error < 0) {
				goto __error;
			}
			AVFrame *gotFrame = av_frame_alloc();
			av_frame_move_ref(gotFrame, m_frame);
			gotList.push_back(gotFrame);
		}
	__error:
		m_error.set_error(error);
		while (!gotList.empty()) {
			AVFrame *n = gotList.front();
			av_frame_free(&n);
			gotList.pop_front();
		}
		return error;
	}
}