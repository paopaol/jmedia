#ifndef JEMDIA_MUXER_H
#define JEMDIA_MUXER_H

#include <error.h>

#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/rational.h>
}

namespace JMedia {

// a wrapper around a single output AVStream
struct OutputStream {
    AVStream*       st;
    AVCodecContext* enc;

    // video
    int           w;
    int           h;
    AVRational    vTimeBase;
    int64_t       vBitRate;
    int           gop;
    AVPixelFormat pixFmt;
};

class Writer {
public:
    Writer(const std::string& filename);

    int  setInputVideoPar(int w, int h, AVRational timeBase, int64_t bitRate,
                          int gop, AVPixelFormat fmt);
    bool hasVideo()
    {
        return m_ofmt->video_codec != AV_CODEC_ID_NONE;
    }

    bool hasAudio()
    {
        return m_ofmt->audio_codec != AV_CODEC_ID_NONE;
    }

    int  open();
    int  write(AVMediaType type, AVPacket* pkt);
    void dumpFormat();

private:
    int addStream(OutputStream* ost, AVFormatContext* oc, AVCodec** codec,
                  AVCodecID id);
    /**
     * @brief    init video muxer paramer
     *
     * @details  detailed description
     *
     * @param    w,h
     * Resolution must be a multiple of two.
     *
     * @param    timeBase
     * timebase: This is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented. For fixed-fps content,
     * timebase should be 1/framerate and timestamp increments should be
     * identical to 1.
     * @return   return type
     */
    int  openVideo(OutputStream* ost, AVCodec* codec);
    void initAudio(OutputStream* ost, AVSampleFormat fmt, int64_t bitRate,
                   int sampleRate, int channels, int channelLayout);
    AVFormatContext* m_fmt_ctx;
    AVOutputFormat*  m_ofmt;
    std::string      m_filename;
    error            m_error;
    OutputStream     m_ovst;
    OutputStream     m_oast;
};
}  // namespace JMedia

#endif /* JEMDIA_MUXER_H */
