//
// Created by jz on 16-12-25.
//

#ifndef DECODE_FILTERGRAPH_N_H
#define DECODE_FILTERGRAPH_N_H

#include <string>


extern "C" {
#include <libavfilter/avfilter.h>
#include <libavutil/samplefmt.h>
#include <libavutil/rational.h>
#include <libavutil/opt.h>
#include <libavfilter/buffersrc.h>
#include <libswscale/swscale.h>
#include <libavfilter/buffersink.h>
}

#include <error.h>
#include <list>
#include <vector>
#include <memory>

namespace JMedia {

	class FilterGraphN;
	class InputFilter {
	private:
		friend class FilterGraphN;



		AVFilterContext *m_filter;
		FilterGraphN     *m_graph;
		std::string      m_name;

		int              m_fmt;
		int              m_w;
		int              m_h;
		AVRational       m_aspect;

		int              m_sample_rate;
		int              m_channels;
		uint64_t         m_channel_layout;

		AVBufferRef      *m_hw_frames_ctx;
		int              eof;
	};

	typedef std::shared_ptr<InputFilter> InputFilterRef;

	class OutputFilter {
	private:




		friend class FilterGraphN;




		AVFilterContext *m_filter;
		FilterGraphN    *m_graph;
		std::string     m_name;
		AVFilterInOut   *m_out_tmp;
		AVMediaType     m_type;

		int             m_w;
		int             m_h;
		AVRational      m_frame_rate;
		int             m_fmt;
		int             m_sample_rate;
		uint64_t        m_channel_layout;

		std::vector<int> m_fmts;
		std::vector<uint64_t> m_channel_layouts;
		std::vector<int>  m_sample_rates;
	};

	typedef std::shared_ptr<OutputFilter> OutputFilterRef;


	class FilterGraphN {
	public:
		FilterGraphN(const std::string &desc);

		void set_input_video_info(AVRational time_base,
			AVRational frame_rate,
			AVRational aspect,
			int w,
			int h,
			int fmt,
			AVBufferRef *hw_frames_ctx);

		int config_input_video_filter(const InputFilterRef &input_filter, AVFilterInOut *in);

		int config_input_filters(const InputFilterRef &input_filter, AVFilterInOut *in);
		

		int config_output_video_filter(const OutputFilterRef &output_filter, AVFilterInOut *out);
		

		int config_output_filter(const OutputFilterRef &output_filter, AVFilterInOut *out);
		
		int graph_init();
		



		int src_add_frame(AVFrame *frame);

		int sink_get_frame(std::list<AVFrame *> &filteredFrameList);


	private:
		AVFilterGraph *m_graph;
		std::string m_graph_desc;
		std::vector<InputFilterRef> m_inputFilters;
		std::vector<OutputFilterRef> m_outputFilters;

		//video
		AVRational                  m_time_base;
		AVRational                  m_frame_rate;
		AVRational                  m_aspect;
		int                         m_w;
		int                         m_h;
		int                         m_fmt;
		AVBufferRef                 *m_hw_frame_ctx;
		AVFrame					*m_frame;
	};

}
#endif