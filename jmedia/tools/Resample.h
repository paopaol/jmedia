//
// Created by jz on 17-1-4.
//

#ifndef DECODE_AUDIO_RESAMPLE_H
#define DECODE_AUDIO_RESAMPLE_H

extern "C"{
#include <libswresample/swresample.h>
#include <libavutil/samplefmt.h>
};

#include "../Error.h"

namespace JMedia {
    struct ResampleConfig{
        int64_t             src_ch_layout;
        int                 src_rate;
        AVSampleFormat      src_sample_fmt;

        int64_t             dst_ch_layout;
        int                 dst_rate;
        AVSampleFormat      dst_sample_fmt;
    };


    class Resample {
    public:
        Resample();

        int init(ResampleConfig &config);

        int convert(const uint8_t **src_data, int src_nb_samples, uint8_t **dst_data, int *dst_linesize);

    private:
        SwrContext                          *m_swr_context;
        ResampleConfig                      m_resample_config;
        uint8_t                             **m_dst_data;
        int                                 m_dst_linesize;
        error                               m_error;
    };
}


#endif //DECODE_AUDIO_RESAMPLE_H
