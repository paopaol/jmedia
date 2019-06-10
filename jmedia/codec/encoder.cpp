//
// Created by jz on 16-12-24.
//

#include "encoder.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>

namespace JMedia {
Encoder::Encoder(AVCodecID id)
    : m_id(id),
      m_codec(nullptr),
      m_codec_context(nullptr),
      m_pkt(nullptr),
      m_fifo(nullptr),
      m_a_pts(0),
      m_opened(false)
{
    m_codec = avcodec_find_encoder(m_id);
    if (m_codec) {
        m_codec_context = avcodec_alloc_context3(m_codec);
    }

    m_pkt = av_packet_alloc();
}

Encoder::~Encoder()
{
    if (m_codec_context) {
        avcodec_free_context(&m_codec_context);
    }
    if (m_pkt) {
        av_packet_free(&m_pkt);
    }
    if (m_fifo) {
        av_audio_fifo_free(m_fifo);
        m_fifo = NULL;
    }
}

int Encoder::set_input_video_attr(AVPixelFormat fmt, int w, int h)
{
    if (!m_codec_context) {
        m_error.set_error(AVERROR_ENCODER_NOT_FOUND);
        return m_error.error_code();
    }

    m_codec_context->width   = w;
    m_codec_context->height  = h;
    m_codec_context->pix_fmt = fmt;
    return 0;
}
int Encoder::set_output_video_attr(int64_t bitRate, AVRational timeBase,
                                   AVRational frameRate, int gop,
                                   int maxBFrames)
{
    if (!m_codec_context) {
        m_error.set_error(AVERROR_ENCODER_NOT_FOUND);
        return m_error.error_code();
    }

    m_codec_context->bit_rate     = bitRate;
    m_codec_context->time_base    = timeBase;
    m_codec_context->framerate    = frameRate;
    m_codec_context->gop_size     = gop;
    m_codec_context->max_b_frames = maxBFrames;
    return 0;
}
int Encoder::set_input_audio_attr(AVSampleFormat fmt, int sampleRate,
                                  int channelLayout)
{
    if (!m_codec_context) {
        m_error.set_error(AVERROR_ENCODER_NOT_FOUND);
        return m_error.error_code();
    }
    m_codec_context->sample_fmt     = fmt;
    m_codec_context->sample_rate    = sampleRate;
    m_codec_context->channel_layout = channelLayout;
    m_codec_context->channels =
        av_get_channel_layout_nb_channels(channelLayout);

    return 0;
}

int Encoder::set_output_audio_attr(int64_t bitRate, AVRational timebase)
{
    if (!m_codec_context) {
        m_error.set_error(AVERROR_ENCODER_NOT_FOUND);
        return m_error.error_code();
    }
    m_codec_context->bit_rate  = bitRate;
    m_codec_context->time_base = timebase;
    return 0;
}

int Encoder::open_once()
{
    if (!m_codec_context) {
        m_error.set_error(AVERROR_ENCODER_NOT_FOUND);
        return m_error.error_code();
    }

    int e = 0;
    if (m_opened) {
        return 0;
    }
    if (m_codec_context->codec_type == AVMEDIA_TYPE_AUDIO) {
        // encoded audio data first must into audio fifo
        m_fifo = av_audio_fifo_alloc(m_codec_context->sample_fmt,
                                     m_codec_context->channels, 1);
        if (!m_fifo) {
            m_error.set_error(AVERROR(ENOMEM));
            return m_error.error_code();
        }
    }
	m_codec_context->sample_aspect_ratio = { 1,1 };

    /* open it */
    e = avcodec_open2(m_codec_context, m_codec, NULL);
    if (e < 0) {
        m_error.set_error(e);
        return e;
    }
    m_opened = true;
    return 0;
}

int Encoder::encode(AVFrame* frame, list<AVPacket*>& encoded_packet_list)
{
    int ret;
    switch (m_codec->type) {
    case AVMEDIA_TYPE_AUDIO: {
        ret = encode_audio(frame, encoded_packet_list);
        break;
    }
    default: {
        ret = encode_encoding(frame, encoded_packet_list);
        break;
    }
    }
    return ret;
}

int Encoder::encode_encoding(AVFrame*         frame,
                             list<AVPacket*>& encoded_packet_list)
{
    int                  error;
    std::list<AVPacket*> packet_list;

    if (!frame || !m_codec_context) {
        error = AVERROR_ENCODER_NOT_FOUND;
        goto error_out;
    }

    /* send the frame for encoding */
    error = avcodec_send_frame(m_codec_context, frame);
    if (error < 0) {
        goto error_out;
    }
    /* read all the available output packets (in general there may be any
     * number of them */
    while (error >= 0) {
        error = avcodec_receive_packet(m_codec_context, m_pkt);
        if (error == AVERROR(EAGAIN) || error == AVERROR_EOF) {
            encoded_packet_list = packet_list;
            return 0;
        }
        if (error < 0) {
            goto error_out;
        }
        AVPacket* pNewPacket = av_packet_alloc();
        av_packet_move_ref(pNewPacket, m_pkt);
        packet_list.push_back(pNewPacket);
    }
	return 0;

error_out:
    m_error.set_error(error);

    while (!packet_list.empty()) {
        AVPacket* node = packet_list.front();
        av_packet_free(&node);
        packet_list.pop_front();
    }
    return error;
}

int Encoder::encode_audio(AVFrame* frame, list<AVPacket*>& encoded_packet_list)
{
    int             output_frame_size = m_codec_context->frame_size;
    list<AVPacket*> pkt_list;

    if (output_frame_size == 0) {
        output_frame_size = frame->nb_samples;
    }

    int error = feedingSamples2Fifo(frame);
    if (error < 0) {
        goto error_out;
    }
    while (av_audio_fifo_size(m_fifo) > output_frame_size) {
        output_frame_size =
            std::min(av_audio_fifo_size(m_fifo), output_frame_size);
        AVFrame* tmp = allocAudioFrame(output_frame_size);
        if (!tmp) {
            error = AVERROR(ENOMEM);
            m_error.set_error(AVERROR(ENOMEM));
            goto error_out;
        }
        tmp->pts = m_a_pts;
        m_a_pts += output_frame_size;
        std::shared_ptr<void> __(nullptr, std::bind([&]() {
                                     av_frame_unref(tmp);
                                     av_frame_free(&tmp);
                                 }));

        int error =
            av_audio_fifo_read(m_fifo, ( void** )tmp->data, output_frame_size);

        if (error < output_frame_size) {
            goto error_out;
        }

        error = encode_encoding(tmp, pkt_list);
        if (error < 0) {
            goto error_out;
        }
        if (pkt_list.size() == 0) {
            continue;
        }
        std::copy(pkt_list.cbegin(), pkt_list.cend(),
                  back_inserter(encoded_packet_list));
    }
    return 0;
error_out:
    m_error.set_error(error);

    while (!pkt_list.empty()) {
        AVPacket* node = pkt_list.front();
        av_packet_free(&node);
        pkt_list.pop_front();
    }
    return error;
}

int Encoder::feedingSamples2Fifo(const AVFrame* frame)
{
    int error;
    if ((error = av_audio_fifo_write(m_fifo, ( void** )frame->data,
                                     frame->nb_samples))
        < 0) {
        goto error_out;
    }
    return 0;
error_out:
    m_error.set_error(error);
    return error;
}

AVFrame* Encoder::allocAudioFrame(int nb_samples)
{
    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        return NULL;
    }

    frame->nb_samples = nb_samples;
    frame->channel_layout =
        av_get_default_channel_layout(m_codec_context->channels);
    frame->format      = m_codec_context->sample_fmt;
    frame->sample_rate = m_codec_context->sample_rate;

    av_frame_get_buffer(frame, 0);
    av_samples_get_buffer_size(frame->linesize, frame->channels, nb_samples,
                               ( AVSampleFormat )frame->format, 1);
    return frame;
}
}  // namespace JMedia
