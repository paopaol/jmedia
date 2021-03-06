//
// Created by jz on 17-1-4.
//

#ifndef DECODE_AUDIO_RESAMPLE_H
#define DECODE_AUDIO_RESAMPLE_H

extern "C" {
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
};

#include <error.h>

namespace JMedia {
struct ResamplerConfig {
    int64_t        src_ch_layout;
    int            src_rate;
    AVSampleFormat src_sample_fmt;

    int64_t        dst_ch_layout;
    int            dst_rate;
    AVSampleFormat dst_sample_fmt;
};

class Resampler {
public:
    Resampler();
    ~Resampler();
    int init_once(ResamplerConfig& config);
    int convert(const uint8_t** src_data, int src_nb_samples);
    int convert(AVFrame* in, AVFrame*& out);
    int get_converted(uint8_t*& data, int& size);

private:
    SwrContext*     m_swr_context;
    AVFrame*        m_frame;
    ResamplerConfig m_resample_config;
    uint8_t**       m_dst_data;
    int             m_dst_linesize;
    error           m_error;
};
}  // namespace JMedia

#endif  // DECODE_AUDIO_RESAMPLE_H
