//
// Created by jz on 16-12-24.
//

#include <assert.h>
#include <functional>
#include <list>
#include <memory>
#include <string>

#include "format_reader.h"
#include <stdio.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/dict.h>
}

#include <Windows.h>

namespace JMedia {

using namespace std;

class ffmpeg_init_struct {
public:
    ffmpeg_init_struct()
    {
        av_register_all();
        avformat_network_init();
        avdevice_register_all();
    }
};

static ffmpeg_init_struct init_struct;

static Duration int64_2_duration(int64_t d)
{
    Duration duration;

    duration.Seconds = ( int )d / AV_TIME_BASE;
    duration.Us      = d % AV_TIME_BASE;
    duration.Minutes = duration.Seconds / 60;
    duration.Seconds %= 60;
    duration.Hours = duration.Minutes / 60;
    duration.Minutes %= 60;

    return duration;
}

class FormatReaderPrivate {
public:
    bool     Timeout;
    MMRESULT TimerId;
};

static void WINAPI timerfunc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl,
                             DWORD dw2)
{
    FormatReaderPrivate* priv = ( FormatReaderPrivate* )dwUser;
    assert(wTimerID == priv->TimerId);
    priv->Timeout = true;
}

int FormatReader::interruput(void* arg)
{
    FormatReaderPrivate* priv = ( FormatReaderPrivate* )arg;
    if (priv->Timeout) {
        return 1;
    }
    return 0;
}

FormatReader::FormatReader(const std::string& filename)
    : m_filename(filename), m_input_format_context(NULL), priv(NULL)
{
}

FormatReader::~FormatReader()
{
    if (priv) {
        delete priv;
    }
}

int FormatReader::close()
{
    for (auto stream = m_streams.begin(); stream != m_streams.end(); stream++) {
        AVCodecContext* codec_context = stream->codec_context;
        if (codec_context) {
            avcodec_free_context(&codec_context);
        }
    }
    if (m_input_format_context) {
        avformat_close_input(&m_input_format_context);
    }
    return 0;
}

map<string, string> FormatReader::metadata()
{
    assert(m_input_format_context != nullptr);

    AVDictionaryEntry*  tag = NULL;
    map<string, string> metadata;

    while ((tag = av_dict_get(m_input_format_context->metadata, "", tag,
                              AV_DICT_IGNORE_SUFFIX))) {
        metadata[tag->key] = tag->value;
    }
    return metadata;
}

Duration FormatReader::duration()
{
    assert(m_input_format_context != nullptr);

    if (m_input_format_context->duration == AV_NOPTS_VALUE) {
        return int64_2_duration(0);
    }

    int64_t d =
        m_input_format_context->duration
        + (m_input_format_context->duration <= INT64_MAX - 5000 ? 5000 : 0);
    Duration duration;

    duration = int64_2_duration(d);
    return duration;
}

Duration FormatReader::start_time()
{
    assert(m_input_format_context != nullptr);

    if (m_input_format_context->start_time == AV_NOPTS_VALUE) {
        return int64_2_duration(0);
    }

    int64_t t =
        m_input_format_context->start_time
        + (m_input_format_context->start_time <= INT64_MAX - 5000 ? 5000 : 0);
    Duration duration;

    duration = int64_2_duration(t);
    return duration;
}

int FormatReader::open(bool dshow)
{
    int           error_code;
    AVCodec*      input_codec = NULL;
    AVDictionary* options     = NULL;
    av_dict_set(&options, "buffer_size", "1024000", 0);
    av_dict_set(&options, "max_delay", "500000", 0);
    av_dict_set(&options, "stimeout", "20000000", 0);  //设置超时断开连接时间
    av_dict_set(&options, "rtsp_transport", "tcp",
                0);  //以udp方式打开，如果以tcp方式打开将udp替换为tcp
    av_dict_set(&options, "framerate", "30", 0);  //设置超时断开连接时间
    av_dict_set(&options, "video_size", "1920x1080", 0);  //设置超时断开连接时间
	//av_dict_set(&options, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);


    AVInputFormat* pInputFmt = NULL;
    if (dshow) {
        pInputFmt = av_find_input_format("dshow");
    }

    m_input_format_context = avformat_alloc_context();
    if (!m_input_format_context) {
        m_error.set_error(AVERROR(ENOMEM));
        return AVERROR(ENOMEM);
    }

    priv          = new FormatReaderPrivate;
    priv->TimerId = timeSetEvent(1000 * 30, 1, ( LPTIMECALLBACK )timerfunc,
                                 DWORD(priv), TIME_ONESHOT);
    priv->Timeout = false;

    m_input_format_context->interrupt_callback.callback = interruput;
    m_input_format_context->interrupt_callback.opaque   = priv;

    std::shared_ptr<void> __(
        nullptr, std::bind([this]() { timeKillEvent(priv->TimerId); }));
    error_code = avformat_open_input(&m_input_format_context,
                                     m_filename.c_str(), pInputFmt, &options);
    if (error_code < 0) {
        m_error.set_error(error_code);
        return error_code;
    }

    error_code = avformat_find_stream_info(m_input_format_context, NULL);
    if (error_code < 0) {
        m_error.set_error(error_code);
        return error_code;
    }
    for (unsigned int i = 0; i < m_input_format_context->nb_streams; i++) {
        AVStream*          stream       = m_input_format_context->streams[i];
        AVMediaType        media_type   = stream->codecpar->codec_type;
        AVCodecID          codec_id     = stream->codecpar->codec_id;
        AVCodecParameters* codecpar     = stream->codecpar;
        int                stream_index = i;

        input_codec = avcodec_find_decoder(codec_id);
        if (!input_codec) {
            continue;
        }

        AVCodecContext* codec_context = avcodec_alloc_context3(input_codec);
        if (!codec_context) {
            m_error.set_error(AVERROR(ENOMEM));
            return AVERROR(ENOMEM);
        }
        error_code = avcodec_parameters_to_context(codec_context, codecpar);
        if (error_code < 0) {
            m_error.set_error(error_code);
            return error_code;
        }
        error_code = avcodec_open2(codec_context, input_codec, NULL);
        if (error_code < 0) {
            m_error.set_error(error_code);
            return error_code;
        }
        Decoder decoder = Decoder(codec_context);
        Stream  stream_ = { media_type, codec_context, decoder, stream_index,
                           stream };
        m_streams.push_back(stream_);
    }
    return 0;
}

int FormatReader::read_packet(Packet& pkt)
{
    int error = 0;

    if (m_input_format_context == nullptr) {
        m_error.set_error(AVERROR_INVALIDDATA);
        return AVERROR_INVALIDDATA;
    }

    error = av_read_frame(m_input_format_context, &pkt.m_pkt);
    if (error < 0) {
        m_error.set_error(error);
        return error;
    }
    AVMediaType t  = media_type(pkt);
    pkt.media_type = t;
    return error;
}

AVMediaType FormatReader::media_type(Packet& pkt)
{
    int stream_index = pkt.m_pkt.stream_index;

    for (auto it = m_streams.begin(); it != m_streams.end(); it++) {
        if (it->stream_index == stream_index) {
            return it->media_type;
        }
    }

    return AVMEDIA_TYPE_UNKNOWN;
}

int FormatReader::find_decoder(AVMediaType media_type, Decoder& decoder)
{
    for (auto it = m_streams.begin(); it != m_streams.end(); it++) {
        if (it->media_type == media_type) {
            decoder = it->decoder;
            return 0;
        }
    }
    m_error.set_error(AVERROR_DECODER_NOT_FOUND);
    return AVERROR_DECODER_NOT_FOUND;
}

int FormatReader::getCodecContext(AVMediaType      media_type,
                                  AVCodecContext*& codecContext)
{
    for (auto it = m_streams.begin(); it != m_streams.end(); it++) {
        if (it->media_type == media_type) {
            codecContext = it->codec_context;
            return 0;
        }
    }
    m_error.set_error(AVERROR_DECODER_NOT_FOUND);
    return AVERROR_DECODER_NOT_FOUND;
}

int FormatReader::find_stream(AVMediaType media_type, Stream& stream)
{
    for (auto it = m_streams.begin(); it != m_streams.end(); it++) {
        if (it->media_type == media_type) {
            stream = *it;
            return 0;
        }
    }
    m_error.set_error(AVERROR_STREAM_NOT_FOUND);
    return AVERROR_STREAM_NOT_FOUND;
}

}  // namespace JMedia
