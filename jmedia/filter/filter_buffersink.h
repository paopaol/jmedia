#ifndef FILTER_BUFFERSINK_H
#define FILTER_BUFFERSINK_H

extern "C" {
#include <libavfilter/avfilter.h>
#include <libavutil/samplefmt.h>
#include <libavutil/rational.h>
}

#include <Error.h>
#include "filter.h"
#include "filter_graph.h"

#include <vector>


namespace JMedia {
	class FilterBuffersink : public Filter {
	public:
		FilterBuffersink(FilterGraph *filter_graph, const std::string &name);
		int set_pix_fmts(const std::vector<AVPixelFormat> pix_fmts);
	};
}
#endif