#ifndef VIDEO_FILTER_BUFFER_H
#define VIDEO_FILTER_BUFFER_H

extern "C" {
#include <libavfilter/avfilter.h>
#include <libavutil/samplefmt.h>
#include <libavutil/rational.h>
}

#include <error.h>
#include "filter.h"
#include "filter_graph.h"

namespace JMedia {
	class FilterBuffer :public Filter {
    public:
        FilterBuffer(FilterGraph *filter_graph, const std::string &name);
		int set_w(int w);
		int set_h(int h);
		int set_pix_fmt(AVPixelFormat fmt);
        int set_time_base(AVRational time_base);
        int set_pixel_aspect(AVRational sar);
		int set_frame_rate(int frame_rate);
	};
}
#endif
