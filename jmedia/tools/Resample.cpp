//
// Created by jz on 17-1-4.
//

#include "Resample.h"

namespace JMedia{
    Resample::Resample()
    {
        m_swr_context = nullptr;
    }

    int Resample::init(ResampleConfig& config)
    {
        m_swr_context = swr_alloc();
        if (m_swr_context == nullptr){
            m_error.set_error(AVERROR(ENOMEM));
            return m_error.error_code();
        }
        /* set options */
        av_opt_set_int(swr_ctx, "in_channel_layout",    config.src_ch_layout, 0);
        av_opt_set_int(swr_ctx, "in_sample_rate",       config.src_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", config.src_sample_fmt, 0);
        av_opt_set_int(swr_ctx, "out_channel_layout",    config.dst_ch_layout, 0);
        av_opt_set_int(swr_ctx, "out_sample_rate",       config.dst_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", config.dst_sample_fmt, 0);
        m_resample_config = config;

        /* initialize the resampling context */
        int error = 0;
        if ((error = swr_init(m_swr_context)) < 0) {
            m_error.set_error(error);
            return error;
        }
        return 0;
    }

    int Resample::convert(const uint8_t **src_data, int src_nb_samples, uint8_t **dst_data, int *dst_linesize)
    {
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

    }
}
