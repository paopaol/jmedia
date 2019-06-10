
#include "filter_buffersink.h"

extern "C"{
#include <libavfilter/buffersrc.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
}

#include <assert.h>

namespace JMedia{
    FilterBuffersink::FilterBuffersink(FilterGraph *filter_graph, const std::string &name):
	Filter(name)
	{
        m_filter = (AVFilter *)avfilter_get_by_name("buffersink");
        m_filter_ctx = avfilter_graph_alloc_filter(filter_graph->getAVFilterGraph(), m_filter, name.c_str());
    }

	int FilterBuffersink::set_pix_fmts(const std::vector<AVPixelFormat> pix_fmts)
	{
		assert(m_filter_ctx != NULL);

		int e = av_opt_set_int_list(m_filter_ctx, "pix_fmts", &pix_fmts[0],
			AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
		if (e != 0) {
			m_error.set_error(e);
		}
		return e;
	}
}
