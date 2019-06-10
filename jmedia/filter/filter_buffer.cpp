
#include "filter_buffer.h"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersrc.h>
//#include <libavfilter/avfiltergraph.h>
#include <libavfilter/avfilter.h>
#include <libavutil/opt.h>
}

#include <assert.h>

#include "Filter.h"

namespace JMedia {
	FilterBuffer::FilterBuffer(FilterGraph *filter_graph, const std::string &name):
		Filter(name)
	{
        m_filter = (AVFilter *)avfilter_get_by_name("buffer");
		m_filter_ctx = avfilter_graph_alloc_filter(filter_graph->getAVFilterGraph(), m_filter, name.c_str());
	}

	int FilterBuffer::set_w(int w)
	{
		assert(m_filter_ctx != NULL);

		int e = av_opt_set_int(m_filter_ctx, "width", w, AV_OPT_SEARCH_CHILDREN);
		if (e != 0) {
			m_error.set_error(e);
		}
		return e;
	}

	int FilterBuffer::set_h(int h)
	{
		assert(m_filter_ctx != NULL);

		int e = av_opt_set_int(m_filter_ctx, "height", h, AV_OPT_SEARCH_CHILDREN);
		if (e != 0) {
			m_error.set_error(e);
		}
		return e;
	}

	int FilterBuffer::set_pix_fmt(AVPixelFormat fmt)
	{
		assert(m_filter_ctx != NULL);

		int e = av_opt_set_pixel_fmt(m_filter_ctx, "pix_fmt", fmt, AV_OPT_SEARCH_CHILDREN);
		if (e != 0) {
			m_error.set_error(e);
		}
		return e;
	}

    int FilterBuffer::set_time_base(AVRational time_base) 
	{
		assert(m_filter_ctx != NULL);

        int e = av_opt_set_q(m_filter_ctx, "time_base", time_base, AV_OPT_SEARCH_CHILDREN);
		if (e != 0) {
			m_error.set_error(e);
		}
        return e;
    }
	int FilterBuffer::set_pixel_aspect(AVRational sar)
	{
		assert(m_filter_ctx != NULL);

        int e = av_opt_set_q(m_filter_ctx, "pixel_aspect", sar, AV_OPT_SEARCH_CHILDREN);
		if (e != 0) {
			m_error.set_error(e);
		}
        return e;
	}

	int FilterBuffer::set_frame_rate(int frame_rate)
	{
		assert(m_filter_ctx != NULL);

		int e = av_opt_set_int(m_filter_ctx, "frame_rate", frame_rate, AV_OPT_SEARCH_CHILDREN);
		if (e != 0) {
			m_error.set_error(e);
		}
		return e;

	}
}