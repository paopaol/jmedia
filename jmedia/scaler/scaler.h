
extern "C"{
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
};

#include <Error.h>


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

		int convert(const AVFrame *frame, AVFrame *&outFrame);
        const char *errors(){
            return m_error.what();
        }

    private:
        SwsContext              *m_sws_context;

        ScalerConfig            m_config;

        //uint8_t                 *m_dst_data[AV_NUM_DATA_POINTERS];

        //int                     m_dst_linesize[AV_NUM_DATA_POINTERS];
		AVFrame					*m_frame;

        int                     m_dst_buffsize;

        error                   m_error;
    };
}