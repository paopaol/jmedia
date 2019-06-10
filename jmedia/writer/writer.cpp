#include "writer.h"

namespace JMedia {
Writer::Writer(const std::string& filename)
    : m_filename(filename), m_fmt_ctx(nullptr), m_ofmt(nullptr)
{
    avformat_alloc_output_context2(&m_fmt_ctx, nullptr, nullptr,
                                   m_filename.c_str());
    if (!m_fmt_ctx) {
        m_error.set_error(AVERROR(ENOMEM));
    }
    m_ofmt = m_fmt_ctx->oformat;
}

int Writer::open()
{
    if (!m_fmt_ctx) {
        return m_error.error_code();
    }

    int e = 0;
    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    if (hasVideo()) {
        // add video
        AVCodec* vcodec = nullptr;
        e = addStream(&m_ovst, m_fmt_ctx, &vcodec, m_ofmt->video_codec);
        if (e < 0) {
            return e;
        }
        e = openVideo(&m_ovst, vcodec);
        if (e < 0) {
            return e;
        }
    }
    if (hasAudio()) {
        // add audio
        AVCodec* acodec = nullptr;
        e = addStream(&m_oast, m_fmt_ctx, &acodec, m_ofmt->audio_codec);
        if (e < 0) {
            m_error.set_error(e);
            return e;
        }
    }

    /* open the output file, if needed */
    if (m_ofmt->flags & AVFMT_NOFILE) {
        e = avio_open(&m_fmt_ctx->pb, m_filename.c_str(), AVIO_FLAG_WRITE);
        if (e < 0) {
            m_error.set_error(e);
            return e;
        }
    }

    /* Write the stream header, if any. */
    // FIXME: options need replace nullptr
    e = avformat_write_header(m_fmt_ctx, nullptr);
    if (e < 0) {
        m_error.set_error(e);
        return e;
    }
    return 0;
}

int Writer::write(AVMediaType type, AVPacket* pkt)
{
    OutputStream* ost = nullptr;

    if (type == AVMEDIA_TYPE_VIDEO && hasVideo()) {
        ost = &m_ovst;
    }
    else if (type == AVMEDIA_TYPE_AUDIO && hasAudio()) {
        ost = &m_oast;
    }

    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, ost->enc->time_base, ost->st->time_base);
    pkt->stream_index = ost->st->index;

    // log_packet(fmt_ctx, pkt);

    /* Write the compressed frame to the media file. */
    return av_interleaved_write_frame(m_fmt_ctx, pkt);
}

void Writer::dumpFormat()
{
    av_dump_format(m_fmt_ctx, 0, m_filename.c_str(), 1);
}

int Writer::addStream(OutputStream* ost, AVFormatContext* oc, AVCodec** codec,
                      AVCodecID codecId)
{
    *codec = avcodec_find_encoder(codecId);
    if (!*codec) {
        m_error.set_error(AVERROR_ENCODER_NOT_FOUND);
        return AVERROR_ENCODER_NOT_FOUND;
    }
    ost->st     = avformat_new_stream(oc, *codec);
    ost->st->id = oc->nb_streams - 1;
    ost->enc    = avcodec_alloc_context3(*codec);
    if (!ost->enc) {
        m_error.set_error(AVERROR(ENOMEM));
        return AVERROR(ENOMEM);
    }
    return 0;
}

void initVideo(OutputStream* ost, AVCodecID id, AVPixelFormat fmt,
               int64_t bitRate, int w, int h, AVRational timeBase, int gop)
{
}

void Writer::initAudio(OutputStream* ost, AVSampleFormat fmt, int64_t bitRate,
                       int sampleRate, int channels, int channelLayout)
{
    AVCodecContext* c = ost->enc;

    c->sample_fmt      = fmt;
    c->bit_rate        = bitRate;
    c->sample_rate     = sampleRate;
    c->channels        = av_get_channel_layout_nb_channels(channelLayout);
    c->channel_layout  = channelLayout;
    ost->st->time_base = { 1, c->sample_rate };

    if (m_ofmt->flags & AVFMT_GLOBALHEADER) {
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
}

int Writer::openVideo(OutputStream* ost, AVCodec* codec)
{
    int             e = 0;
    AVCodecContext* c = ost->enc;

    c->codec_id        = m_ofmt->video_codec;
    c->bit_rate        = ost->vBitRate;
    c->width           = ost->w;
    c->height          = ost->h;
    ost->st->time_base = ost->vTimeBase;
    c->time_base       = ost->vTimeBase;
    c->gop_size        = ost->gop;

    if (m_ofmt->flags & AVFMT_GLOBALHEADER) {
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    /* open the codec */
    e = avcodec_open2(c, codec, NULL);
    if (e < 0) {
        m_error.set_error(e);
        return e;
    }
    /* copy the stream parameters to the muxer */
    e = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (e < 0) {
        m_error.set_error(e);
        return e;
    }
    return 0;
}

int Writer::setInputVideoPar(int w, int h, AVRational timeBase, int64_t bitRate,
                             int gop, AVPixelFormat fmt)
{
    const AVPixelFormat* supportList = m_ovst.enc->codec->pix_fmts;
    bool                 support     = false;
    for (int i = 0; supportList[i] != AV_PIX_FMT_NONE; i++) {
        if (fmt == supportList[i]) {
            support = true;
            break;
        }
    }
    if (!support) {
        m_error.set_error(AVERROR_INVALIDDATA);
        return AVERROR_INVALIDDATA;
    }
    m_ovst.w         = w;
    m_ovst.h         = h;
    m_ovst.vTimeBase = timeBase;
    m_ovst.vBitRate  = bitRate;
    m_ovst.gop       = gop;
    m_ovst.pixFmt    = fmt;

    return 0;
}

// int main()
// {
//     Writer file("1.h264");

//     if (file.hasVideo()) {
//         file.setInputVideoPar(288, 352, { 1, 25 }, 40000, 12,
//                               AV_PIX_FMT_YUV420P);
//     }
// 	file.open();

// 	while(1){
// 		file.write(pkt);
// 	}

// 	file.close();;

// }

}  // namespace JMedia
