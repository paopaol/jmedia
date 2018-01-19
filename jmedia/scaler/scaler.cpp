

extern "C"{
#include <libavutil/imgutils.h>
};

#include "scaler.h"

namespace JMedia{
	Scaler::Scaler() :
		m_sws_context(NULL),
		m_frame(NULL)
    {
        m_sws_context = nullptr;
		m_frame = av_frame_alloc();

    }

    Scaler::~Scaler()
    {
		if (m_frame) {
            av_freep(&m_frame->data[0]);
			av_frame_free(&m_frame);
		}

        if (m_sws_context){
            sws_freeContext(m_sws_context);
        }
    }

    int Scaler::init_once(ScalerConfig &config)
    {
        int         error = 0;

        m_config = config;

        if (m_sws_context == nullptr){
			m_frame->width = config.dst_width;
			m_frame->height = config.dst_height;
			m_frame->format = config.dst_pix_fmt;

			m_sws_context = sws_getContext(config.src_width, config.src_height, config.src_pix_fmt,
				config.dst_width, config.dst_height, config.dst_pix_fmt,
				SWS_BICUBIC, NULL, NULL, NULL);
			if (m_sws_context == nullptr) {
				error = AVERROR(EINVAL);
				m_error.set_error(error);
				return error;
			}

			error = av_image_alloc(m_frame->data, m_frame->linesize,
				config.dst_width, config.dst_height, config.dst_pix_fmt, 1);
			if (error < 0) {
				m_error.set_error(error);
				return error;
			}
			m_dst_buffsize = error;
        }
        return 0;
    }

    int Scaler::scale(const AVFrame *in, AVFrame *&out)
    {
        int         error = 0;

        error = sws_scale(m_sws_context, 
			in->data, in->linesize, 0, m_config.src_height, 
			m_frame->data, m_frame->linesize);
        if (error < 0){
            m_error.set_error(error);
            return error;
        }
		AVFrame *v = av_frame_alloc();
        av_frame_unref(v);
		av_frame_move_ref(v, m_frame);
		out = v;
        return 0;
    }
}