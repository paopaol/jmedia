

extern "C"{
#include <libavutil/imgutils.h>
};

#include "Scaler.h"

namespace JMedia{
    Scaler::Scaler()
    {
        m_sws_context = nullptr;
        memset(m_dst_data, 0, sizeof(m_dst_data));
        memset(m_dst_linesize, 0, sizeof(m_dst_linesize));
    }

    Scaler::~Scaler()
    {
        if (m_dst_data[0]){
            av_freep(&m_dst_data[0]);
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
           m_sws_context = sws_getContext(config.src_width, config.src_height, config.src_pix_fmt,
                            config.dst_width, config.dst_height, config.dst_pix_fmt,
                            SWS_BILINEAR, NULL, NULL, NULL);
           if (m_sws_context == nullptr){
               error = AVERROR(EINVAL);
               m_error.set_error(error);
               return error;
           }

           error = av_image_alloc(m_dst_data, m_dst_linesize, 
                                    config.dst_width, config.dst_height, config.dst_pix_fmt, 1);
           if (error < 0){
               m_error.set_error(error);
               return error;
           }
           m_dst_buffsize = error;
        }
        return 0;
    }

    int Scaler::convert(const uint8_t **src_data, int linesize[])
    {
        int         error = 0;

        error = sws_scale(m_sws_context, src_data, linesize, 0, m_config.src_height, m_dst_data, m_dst_linesize);
        if (error < 0){
            m_error.set_error(error);
            return error;
        }
        return 0;
    }

    int Scaler::get_converted(uint8_t *&data, int &size)
    {
        data = m_dst_data[0];
        size = m_dst_buffsize;
        return 0;
    }

}