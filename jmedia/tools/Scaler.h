
extern "C"{
#include <libswscale/swscale.h>
};

#include "../Error.h"


namespace JMedia{
    struct ScalerConfig{
        int             src_width;
        int             src_height;
        AVPixelFormat   src_pix_fmt;

        int             dst_width;
        int             dst_height;
        AVPixelFormat   dst_pix_fmt;

    };

    class Scaler{
    public:
        Scaler();

        ~Scaler();

        int init_once(ScalerConfig &config);

        int convert(const uint8_t **src_data,  int linesize[]);

        int get_converted(uint8_t *&data, int &size);

    private:
        SwsContext              *m_sws_context;

        ScalerConfig            m_config;

        uint8_t                 *m_dst_data[4];

        int                     m_dst_linesize[4];

        int                     m_dst_buffsize;

        error                   m_error;
    };
}