#include <filter_graph_n.h>


namespace JMedia {

		FilterGraphN::FilterGraphN(const std::string &desc)
			:m_graph(nullptr)
			, m_graph_desc(desc)
			,m_hw_frame_ctx(nullptr)
		
		{
			m_graph = avfilter_graph_alloc();
			m_graph->nb_threads = 0;
			m_graph->scale_sws_opts = av_strdup("flags=bicubic");
			av_opt_set(m_graph, "aresample_swr_opts", "", 0);
			m_frame = av_frame_alloc();
			InputFilterRef in = std::make_shared<InputFilter>();
			m_inputFilters.push_back(in);
			OutputFilterRef out = std::make_shared<OutputFilter>();
			m_outputFilters.push_back(out);
		}

		void FilterGraphN::set_input_video_info(AVRational time_base,
			AVRational frame_rate,
			AVRational aspect,
			int w,
			int h,
			int fmt,
			AVBufferRef *hw_frames_ctx)
		{
			m_time_base = time_base;
			m_frame_rate = frame_rate;
			m_aspect = aspect;
			m_w = w;
			m_h = h;
			m_fmt = fmt;
			m_hw_frame_ctx = av_buffer_ref(hw_frames_ctx);
		}

		int FilterGraphN::config_input_video_filter(const InputFilterRef &input_filter, AVFilterInOut *in)
		{
			int ret = 0;
			const AVFilter *buffer_filter = avfilter_get_by_name("buffer");
			AVBufferSrcParameters *par = av_buffersrc_parameters_alloc();
			
			if (!par) {
				return -1;
			}
			memset(par, 0, sizeof(*par));
			par->format = AV_PIX_FMT_NONE;

			char args[4096] = { 0 };
			sprintf(args, "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:"
				"pixel_aspect=%d/%d:sws_param=flags=%d:frame_rate=%d/%d",
				m_w, m_h, m_fmt, m_time_base.num, m_time_base.den,
				m_aspect.num, m_aspect.den, SWS_BILINEAR,
				m_frame_rate.num, m_frame_rate.den
				);
			char name[1024] = { 0 };
			sprintf(name, "graph %p input ", this);

			ret = avfilter_graph_create_filter(&input_filter->m_filter, 
				buffer_filter, name, args, nullptr, m_graph);
			if (ret < 0) {
				return -1;
			}
			par->hw_frames_ctx = m_hw_frame_ctx;
			ret = av_buffersrc_parameters_set(input_filter->m_filter, par);
			if (ret < 0) {
				return -1;
			}
			av_freep(&par);
			ret = avfilter_link(input_filter->m_filter, 0, in->filter_ctx, in->pad_idx);
			if (ret < 0) {
				return -1;
			}
			return 0;
		}

		int FilterGraphN::config_input_filters(const InputFilterRef &input_filter, AVFilterInOut *in)
		{
			switch (avfilter_pad_get_type(in->filter_ctx->input_pads, in->pad_idx)) {
			case AVMEDIA_TYPE_VIDEO:
				return config_input_video_filter(input_filter, in);
			default:
				return -1;
			}
			return 0;

		}

		int FilterGraphN::config_output_video_filter(const OutputFilterRef &output_filter, AVFilterInOut *out)
		{
			int ret = 0;
			char name[1024] = { 0 };
			AVFilterContext *last_filter = out->filter_ctx;
			int pad_idx = out->pad_idx;

			sprintf(name, "out_%p", this);

			ret = avfilter_graph_create_filter(&output_filter->m_filter,
				avfilter_get_by_name("buffersink"), name, nullptr, nullptr, m_graph);
			if (ret < 0) {
				return -1;
			}
			//char *pix_fmts = "nv12|p010le|qsv";
			//if (pix_fmts) {
			//	AVFilterContext *filter;

			//	sprintf(name, "format_out_%p", this);
			//	ret = avfilter_graph_create_filter(&filter, avfilter_get_by_name("format"),
			//		"format", pix_fmts, nullptr, m_graph);
			//	if (ret < 0) {
			//		return 0;
			//	}
			//	ret = avfilter_link(last_filter, pad_idx, filter, 0);
			//	if (ret < 0) {
			//		return -1;
			//	}
			//	last_filter = filter;
			//	pad_idx = 0;
			//}

			ret = avfilter_link(last_filter, pad_idx, output_filter->m_filter, 0);
			if (ret < 0) {
				return -1;
			}
			return 0;
		}

		int FilterGraphN::config_output_filter(const OutputFilterRef &output_filter, AVFilterInOut *out)
		{
			switch (avfilter_pad_get_type(out->filter_ctx->output_pads, out->pad_idx)) {
			case AVMEDIA_TYPE_VIDEO:
				return config_output_video_filter(output_filter, out);
			default:
				return -1;
			}
			return 0;
		}
		int FilterGraphN::graph_init()
		{
			AVFilterInOut *inputs = nullptr, *outputs = nullptr, *cur = nullptr;

			int ret = avfilter_graph_parse2(m_graph, m_graph_desc.c_str(),
				&inputs, &outputs);
			if (ret < 0) {
				return ret;
			}

			int i = 0;
			for (cur = inputs, i = 0; cur; cur = cur->next, i++) {
				ret = config_input_filters(m_inputFilters[i], cur);
				if (ret < 0) {
					return ret;
				}
			}
			avfilter_inout_free(&inputs);

			for (cur = outputs, i = 0; cur; cur = cur->next, i++) {
				ret = config_output_filter(m_outputFilters[i], cur);
			}
			avfilter_inout_free(&outputs);
			if ((ret = avfilter_graph_config(m_graph, NULL)) < 0) {
				return -1;
			}


			for (int i = 0; i < m_outputFilters.size(); i++) {
				OutputFilterRef ofilter = m_outputFilters[i];
				AVFilterContext *sink = ofilter->m_filter;

				ofilter->m_fmt = av_buffersink_get_format(sink);
				ofilter->m_w = av_buffersink_get_w(sink);
				ofilter->m_h = av_buffersink_get_h(sink);
				ofilter->m_sample_rate = av_buffersink_get_sample_rate(sink);
				ofilter->m_channel_layout = av_buffersink_get_channel_layout(sink);
			}
			return 0;
		}



		int FilterGraphN::src_add_frame(AVFrame *frame) {
			int error = av_buffersrc_add_frame_flags(m_inputFilters[0]->m_filter, frame, AV_BUFFERSRC_FLAG_KEEP_REF);
			//int error = av_buffersrc_add_frame(m_src, frame);
			return error;
		}

		int FilterGraphN::sink_get_frame(std::list<AVFrame *> &filteredFrameList) {
			int error;
			std::list<AVFrame *> gotList;

			while (1) {
				error = av_buffersink_get_frame(m_outputFilters[0]->m_filter, m_frame);
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
			while (!gotList.empty()) {
				AVFrame *n = gotList.front();
				av_frame_free(&n);
				gotList.pop_front();
			}
			return error;
		}
}
