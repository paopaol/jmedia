#ifndef JEMDIA_MUXER_H
#define JEMDIA_MUXER_H

#include <jmedia/error.h>

#include <string>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/rational.h>
}

namespace JMedia {

// a wrapper around a single output AVStream
struct OutputStream {
    OutputStream()
        : st(nullptr),
          enc(nullptr),
          codec(nullptr),
          w(-1),
          h(-1),
          vTimeBase({0, 0}),
          vBitRate(0),
          gop(0),
          pixFmt(AV_PIX_FMT_NONE),
          sampleFmt(AV_SAMPLE_FMT_NONE),
          aTimeBase({0, 0}),
          aBitRate(0),
          sampleRate(0),
          channelLayout(0)
    {
    }

    AVStream*         st;
    AVCodecContext*   enc;
    AVCodec*          codec;
    std::vector<char> extra_data;

    // video
    int           w;
    int           h;
    AVRational    vTimeBase;
    int64_t       vBitRate;
    int           gop;
    AVPixelFormat pixFmt;

    // audio
    AVSampleFormat sampleFmt;
    AVRational     aTimeBase;
    int64_t        aBitRate;
    int            sampleRate;
    int            channelLayout;
};

class FormatWriter {
public:
    FormatWriter(const std::string& filename);
    ~FormatWriter();

    std::vector<AVSampleFormat> supported_sample_fmts();
    std::vector<int>            supported_samplerates();
    std::vector<uint64_t>       supported_channel_layouts();
    std::vector<AVPixelFormat>  supported_pix_fmts();
    std::vector<AVRational>     supported_framerates();

    void      set_input_video_attr(int w, int h, AVRational timeBase,
                                   int64_t bitRate, int gop, AVPixelFormat fmt);
    void      set_input_video_extra_data(const char* data, int size);
    void      set_input_audio_attr(AVRational timeBase, AVSampleFormat fmt,
                                   int64_t bitRate, int sampleRate,
                                   int channelLayout);
    void      set_input_audio_extra_data(const char* data, int size);
    AVCodecID video_id();
    AVCodecID audio_id();
    bool      has_video()
    {
        return m_ofmt->video_codec != AV_CODEC_ID_NONE;
    }

    bool has_audio()
    {
        return m_ofmt->audio_codec != AV_CODEC_ID_NONE;
    }

    int         open();
    int         init();
    int         write(AVMediaType type, AVPacket* pkt);
    int         close();
    void        dump_format();
    const char* errors()
    {
        return m_error.what();
    }

private:
    int add_stream(OutputStream* ost, AVFormatContext* oc, AVCodec** codec,
                   AVCodecID id);
    int open_video(OutputStream* ost);
    int open_audio(OutputStream* ost);
    int open_codec(OutputStream* ost, AVCodec* codec);

    AVFormatContext* m_fmt_ctx;
    AVOutputFormat*  m_ofmt;
    std::string      m_filename;
    error            m_error;
    OutputStream     m_ovst;
    OutputStream     m_oast;
    bool             m_opened;
    std::string      m_fmt;
};
}  // namespace JMedia

#endif /* JEMDIA_MUXER_H */
