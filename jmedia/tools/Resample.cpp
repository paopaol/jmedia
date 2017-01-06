//
// Created by jz on 17-1-4.
//

#include "Resample.h"

namespace JMedia{
    Resampler::Resampler()
    {
        m_swr_context = swr_alloc();
    }

    Resampler::~Resampler()
    {
        if (m_dst_data){
            av_free(&m_dst_data[0]);
        }


        if (m_swr_context){
            swr_free(&m_swr_context);
        }
    }

    int Resampler::init_once(ResampleConfig& config)
    {
        int error = 0;

        if (m_swr_context == nullptr){
            m_error.set_error(AVERROR(ENOMEM));
            return m_error.error_code();
        }

        if (swr_is_initialized(m_swr_context)){
            return 0;
        }

        av_opt_set_int(swr_ctx, "in_channel_layout",    config.src_ch_layout, 0);
        av_opt_set_int(swr_ctx, "in_sample_rate",       config.src_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", config.src_sample_fmt, 0);
        av_opt_set_int(swr_ctx, "out_channel_layout",    config.dst_ch_layout, 0);
        av_opt_set_int(swr_ctx, "out_sample_rate",       config.dst_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", config.dst_sample_fmt, 0);
        m_resample_config = config;

        if ((error = swr_init(m_swr_context)) < 0) {
            m_error.set_error(error);
            return error;
        }
        return 0;
    }

    int Resampler::convert(const uint8_t **src_data, int src_nb_samples)
    {
        int error = 0;

        if (m_swr_context == nullptr){
            m_error.set_error(AVERROR(ENOMEM));
            return m_error.error_code();
        }

        int src_nb_channels = av_get_channel_layout_nb_channels(m_resample_config.src_ch_layout);
        int dst_nb_channels = av_get_channel_layout_nb_channels(m_resample_config.dst_ch_layout);

        int max_dst_nb_samples = 0;
        int dst_nb_samples = 0;

        max_dst_nb_samples = dst_nb_samples = av_rescale_rnd(src_nb_samples, m_resample_config.dst_rate, m_resample_config.src_rate, AV_ROUND_UP);

        if (m_dst_data == nullptr){
            int ret = av_samples_alloc_array_and_samples(&m_dst_data, &m_dst_linesize, dst_nb_channels, dst_nb_samples, m_resample_config.dst_sample_fmt, 0);
            if (ret < 0){
                m_error.set_error(ret);
                return ret;
            }
        }
        dst_nb_samples = av_rescale_rnd(swr_get_delay(m_swr_context, m_resample_config.src_rate) + src_nb_samples, 
                                    m_resample_config.dst_rate, m_resample_config.src_rate,
                                    AV_ROUND_UP);
        if (dst_nb_samples > max_dst_nb_samples){
            av_free(&m_dst_data[0]);
            error = av_samples_alloc(m_dst_data, &m_dst_linesize, 
                                dst_nb_channels, dst_nb_samples, m_resample_config.dst_sample_fmt,
                                1);
            if (error < 0){
                m_error.set_error(error);
                return error;
            }
            max_dst_nb_samples = dst_nb_samples;
        }

        error = swr_convert(m_swr_context, m_dst_data, dst_nb_channels, src_data, src_nb_samples);
        if (error < 0){
            m_error.set_error(error);
            return error;
        }

        int dst_bufsize = av_samples_get_buffer_size(&m_dst_linesize, dst_nb_channels, error, m_resample_config.dst_sample_fmt, 1);
        if (dst_bufsize < 0){
            m_error.set_error(error);
            return error;
        }
        return 0;
    }

    Resampler::get_converted(uint8_t *&data, int &size)
    {
        data = m_dst_data[0];
        size = m_dst_linesize[0];
    }
}
