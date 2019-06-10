//
// Created by jz on 16-12-24.
//

#ifndef ENCODE_AUDIO_JMEDIAENCODER_H
#define ENCODE_AUDIO_JMEDIAENCODER_H

#include <list>
#include <string>
#include <vector>

using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
};

#include <error.h>

namespace JMedia {

class Encoder {
public:
    Encoder(AVCodecID id);
    ~Encoder();
    int set_input_audio_attr(AVSampleFormat fmt, int sampleRate,
                             int channelLayout);
    int set_output_audio_attr(int64_t bitRate, AVRational timebase);

    int set_input_video_attr(AVPixelFormat fmt, int w, int h);
    int set_output_video_attr(int64_t bitRate, AVRational timeBase,
                              AVRational frameRate, int gop, int maxBFrames);
    int open_once();
    int encode(AVFrame* frame, list<AVPacket*>& encoded_packet_list);

    const char* errors()
    {
        return m_error.what();
    }

protected:
    error m_error;

private:
    int encode_encoding(AVFrame* frame, list<AVPacket*>& encoded_packet_list);
    int encode_audio(AVFrame* frame, list<AVPacket*>& encoded_packet_list);
    int feedingSamples2Fifo(const AVFrame* frame);
    AVFrame* allocAudioFrame(int nb_samples);

    AVCodecID       m_id;
    AVCodec*        m_codec;
    AVCodecContext* m_codec_context;
    AVPacket*       m_pkt;
    bool            m_opened;
    AVAudioFifo*    m_fifo;
    int64_t         m_a_pts;
};
}  // namespace JMedia

#endif  // ENCODE_AUDIO_JMEDIAENCODER_H
