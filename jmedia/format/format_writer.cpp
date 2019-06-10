#include "format_writer.h"

#include <functional>
#include <memory>

#include <assert.h>

extern "C" {
#include <libavutil/rational.h>
#include <libavutil/timestamp.h>
}

namespace JMedia {
static bool startWith(const std::string& str, const std::string& with)
{
    if (str.size() >= with.size()
        && memcmp(with.c_str(), str.c_str(), with.size()) == 0) {
        return true;
    }
    return false;
}

FormatWriter::FormatWriter(const std::string& filename)
    : m_filename(filename), m_fmt_ctx(nullptr), m_ofmt(nullptr), m_opened(false)
{
    av_register_all();
    avformat_network_init();

    if (startWith(m_filename.c_str(), "rtmp://")) {
        m_fmt = "flv";
    }
	char *fmt = m_fmt.empty() ? nullptr : (char *)m_fmt.c_str();

    int e = avformat_alloc_output_context2(&m_fmt_ctx, nullptr, fmt,
                                           m_filename.c_str());
    if (!m_fmt_ctx) {
        m_error.set_error(e);
    }
    m_ofmt = m_fmt_ctx->oformat;
    if (m_fmt == "flv") {
        m_ofmt->video_codec = AV_CODEC_ID_H264;
        m_ofmt->audio_codec = AV_CODEC_ID_AAC;
    }
}

FormatWriter::~FormatWriter()
{
    /* Close each codec. */
    if (has_video() && m_ovst.enc) {
        avcodec_free_context(&m_ovst.enc);
    }
    if (has_audio() && m_oast.enc) {
        avcodec_free_context(&m_oast.enc);
    }

    if (m_fmt_ctx) {
        /* free the stream */
        avformat_free_context(m_fmt_ctx);
        m_fmt_ctx = nullptr;
    }
}

int FormatWriter::open()
{
    if (m_opened) {
        return 0;
    }

    if (!m_fmt_ctx) {
        return m_error.error_code();
    }

    std::shared_ptr<void> __(nullptr, std::bind([&]() { m_opened = true; }));

    int e = 0;
    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    if (has_video()) {
        // add video
        e = add_stream(&m_ovst, m_fmt_ctx, &m_ovst.codec, m_ofmt->video_codec);
        if (e < 0) {
            return e;
        }
    }
    if (has_audio()) {
        // add audio
        e = add_stream(&m_oast, m_fmt_ctx, &m_oast.codec, m_ofmt->audio_codec);
        if (e < 0) {
            m_error.set_error(e);
            return e;
        }
    }
    return 0;
}
int FormatWriter::init()
{
    if (!m_fmt_ctx) {
        return m_error.error_code();
    }

    int e = 0;
    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    if (has_video()) {
        e = open_video(&m_ovst);
        if (e < 0) {
            return e;
        }

        e = open_codec(&m_ovst, m_ovst.codec);
        if (e < 0) {
            return e;
        }
    }
    if (has_audio()) {
        e = open_audio(&m_oast);
        if (e < 0) {
            m_error.set_error(e);
            return e;
        }

        e = open_codec(&m_oast, m_oast.codec);
        if (e < 0) {
            return e;
        }
    }

    /* open the output file, if needed */
    if (!(m_ofmt->flags & AVFMT_NOFILE)) {
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

int FormatWriter::close()
{
    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(m_fmt_ctx);

    /* Close each codec. */
    if (has_video()) {
        avcodec_free_context(&m_ovst.enc);
    }
    if (has_audio()) {
        avcodec_free_context(&m_oast.enc);
    }

    if (!(m_fmt_ctx->flags & AVFMT_NOFILE)) {
        /* Close the output file. */
        avio_closep(&m_fmt_ctx->pb);
    }

    /* free the stream */
    avformat_free_context(m_fmt_ctx);
    m_fmt_ctx = nullptr;
    m_opened  = false;
    return 0;
}

#define av_ts2str_cpp(buf, ts) av_ts_make_string(buf, ts)
#define av_ts2timestr_cpp(buf, ts, tb) av_ts_make_time_string(buf, ts, tb)

static void log_packet(const AVFormatContext* fmt_ctx, const AVPacket* pkt)
{
    AVRational* time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    char av_ts1[AV_TS_MAX_STRING_SIZE] = { 0 };
    char av_ts2[AV_TS_MAX_STRING_SIZE] = { 0 };
    char av_ts3[AV_TS_MAX_STRING_SIZE] = { 0 };
    char av_ts4[AV_TS_MAX_STRING_SIZE] = { 0 };
    char av_ts5[AV_TS_MAX_STRING_SIZE] = { 0 };
    char av_ts6[AV_TS_MAX_STRING_SIZE] = { 0 };
    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s "
           "stream_index:%d\n",
           av_ts2str_cpp(av_ts1, pkt->pts),
           av_ts2timestr_cpp(av_ts4, pkt->pts, time_base),
           av_ts2str_cpp(av_ts2, pkt->dts),
           av_ts2timestr_cpp(av_ts5, pkt->dts, time_base),
           av_ts2str_cpp(av_ts3, pkt->duration),
           av_ts2timestr_cpp(av_ts6, pkt->duration, time_base),
           pkt->stream_index);
}

int FormatWriter::write(AVMediaType type, AVPacket* pkt)
{
    OutputStream* ost = nullptr;
    int           e   = 0;

    if (type == AVMEDIA_TYPE_VIDEO && has_video()) {
        ost = &m_ovst;
    }
    else if (type == AVMEDIA_TYPE_AUDIO && has_audio()) {
        ost = &m_oast;
    }

    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, ost->enc->time_base, ost->st->time_base);
    pkt->stream_index = ost->st->index;

    log_packet(m_fmt_ctx, pkt);

    /* Write the compressed frame to the media file. */
    e = av_interleaved_write_frame(m_fmt_ctx, pkt);
    if (e < 0) {
        m_error.set_error(e);
        return e;
    }
    return 0;
}

void FormatWriter::dump_format()
{
    av_dump_format(m_fmt_ctx, 0, m_filename.c_str(), 1);
}

int FormatWriter::add_stream(OutputStream* ost, AVFormatContext* oc,
                             AVCodec** codec, AVCodecID codecId)
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

int FormatWriter::open_codec(OutputStream* ost, AVCodec* codec)
{
    AVCodecContext* c = ost->enc;

    /* open the codec */
    int e = avcodec_open2(c, codec, NULL);
    if (e < 0) {
        m_error.set_error(e);
        return e;
    }
    if (ost->extra_data.size() > 0) {
        int size          = ost->extra_data.size();
        c->extradata_size = size;
        c->extradata =
            ( uint8_t* )av_malloc(size + AV_INPUT_BUFFER_PADDING_SIZE);
        memcpy(c->extradata, ost->extra_data.data(), size);
    }
    /* copy the stream parameters to the muxer */
    e = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (e < 0) {
        m_error.set_error(e);
        return e;
    }
    if (ost->extra_data.size() > 0) {
        int size                       = ost->extra_data.size();
        ost->st->codec->extradata_size = size;
        ost->st->codec->extradata =
            ( uint8_t* )av_malloc(size + AV_INPUT_BUFFER_PADDING_SIZE);
        memcpy(ost->st->codec->extradata, ost->extra_data.data(), size);
    }

    return 0;
}
int FormatWriter::open_video(OutputStream* ost)
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
    c->pix_fmt         = ost->pixFmt;

    ost->st->codec->codec_tag = 0;
    if (m_ofmt->flags & AVFMT_GLOBALHEADER) {
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    return 0;
}

int FormatWriter::open_audio(OutputStream* ost)
{
    AVCodecContext* c = ost->enc;

    c->sample_fmt      = ost->sampleFmt;
    c->bit_rate        = ost->aBitRate;
    c->sample_rate     = ost->sampleRate;
    c->channels        = av_get_channel_layout_nb_channels(ost->channelLayout);
    c->channel_layout  = ost->channelLayout;
    ost->st->time_base = { 1, c->sample_rate };
    c->time_base       = ost->aTimeBase;

    if (m_ofmt->flags & AVFMT_GLOBALHEADER) {
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    return 0;
}

void FormatWriter::set_input_video_extra_data(const char* data, int size)
{
    m_ovst.extra_data.resize(size);
    memcpy(m_ovst.extra_data.data(), data, size);
}

void FormatWriter::set_input_video_attr(int w, int h, AVRational timeBase,
                                        int64_t bitRate, int gop,
                                        AVPixelFormat fmt)
{
    m_ovst.w         = w;
    m_ovst.h         = h;
    m_ovst.vTimeBase = timeBase;
    m_ovst.vBitRate  = bitRate;
    m_ovst.gop       = gop;
    m_ovst.pixFmt    = fmt;
}

void FormatWriter::set_input_audio_extra_data(const char* data, int size)
{
    m_oast.extra_data.resize(size);
    memcpy(m_oast.extra_data.data(), data, size);
}

void FormatWriter::set_input_audio_attr(AVRational timeBase, AVSampleFormat fmt,
                                        int64_t bitRate, int sampleRate,
                                        int channelLayout)
{
    m_oast.sampleFmt     = fmt;
    m_oast.aBitRate      = bitRate;
    m_oast.aTimeBase     = timeBase;
    m_oast.sampleRate    = sampleRate;
    m_oast.channelLayout = channelLayout;
}

std::vector<AVSampleFormat> FormatWriter::supported_sample_fmts()
{
    if (!has_audio()) {
        return {};
    }
    const AVSampleFormat* fmts = m_oast.enc->codec->sample_fmts;
    if (!fmts) {
        return {};
    }
    std::vector<AVSampleFormat> li;
    for (int i = 0; fmts[i] != -1; i++) {
        li.push_back(fmts[i]);
    }
    return li;
}
std::vector<int> FormatWriter::supported_samplerates()
{
    if (!has_audio()) {
        return {};
    }
    const int* samplerates = m_oast.enc->codec->supported_samplerates;
    if (!samplerates) {
        return {};
    }
    std::vector<int> li;
    for (int i = 0; samplerates[i] != 0; i++) {
        li.push_back(samplerates[i]);
    }
    return li;
}
std::vector<uint64_t> FormatWriter::supported_channel_layouts()
{
    if (!has_audio()) {
        return {};
    }
    const uint64_t* layouts = m_oast.enc->codec->channel_layouts;
    if (!layouts) {
        return {};
    }
    std::vector<uint64_t> li;
    for (int i = 0; layouts[i] != 0; i++) {
        li.push_back(layouts[i]);
    }
    return li;
}
std::vector<AVPixelFormat> FormatWriter::supported_pix_fmts()
{
    if (!has_video()) {
        return {};
    }
    const AVPixelFormat* fmts = m_ovst.enc->codec->pix_fmts;
    if (!fmts) {
        return {};
    }
    std::vector<AVPixelFormat> li;
    for (int i = 0; fmts[i] != -1; i++) {
        li.push_back(fmts[i]);
    }
    return li;
}
std::vector<AVRational> FormatWriter::supported_framerates()
{
    if (!has_video()) {
        return {};
    }
    const AVRational* framerates = m_ovst.enc->codec->supported_framerates;
    if (!framerates) {
        return {};
    }
    std::vector<AVRational> li;
    for (int i = 0;; i++) {
        if (framerates[i].den == 0 && framerates[i].num == 0) {
            break;
        }
        li.push_back(framerates[i]);
    }
    return li;
}

AVCodecID FormatWriter::video_id()
{
    if (has_video()) {
        return m_ovst.enc->codec->id;
    }
    return AV_CODEC_ID_NONE;
}

AVCodecID FormatWriter::audio_id()
{

    if (has_audio()) {
        return m_oast.enc->codec->id;
    }
    return AV_CODEC_ID_NONE;
}

}  // namespace JMedia
