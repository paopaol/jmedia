#ifndef JMEDIA_SCALER_H
#define JMEDIA_SCALER_H

extern "C" {
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
};

#include <error.h>

namespace JMedia {
struct ScalerConfig {
    int           src_width;
    int           src_height;
    AVPixelFormat src_pix_fmt;

    int           dst_width;
    int           dst_height;
    AVPixelFormat dst_pix_fmt;
};

class Scaler {
public:
    Scaler();

    ~Scaler();

    int init_once(ScalerConfig& config);

    int         scale(const AVFrame* in, AVFrame*& out);
    AVFrame*    get_scaled();
    const char* errors()
    {
        return m_error.what();
    }

private:
    SwsContext*  m_sws_context;
    ScalerConfig m_config;
    AVFrame*     m_frame;
    int          m_dst_buffsize;
    error        m_error;
};
}  // namespace JMedia

#endif
