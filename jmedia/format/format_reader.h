//
// Created by jz on 16-12-24.
//

#ifndef DECODE_AUDIO_JMEDIAREADER_H
#define DECODE_AUDIO_JMEDIAREADER_H

#include "Reader.h"
#include <exception>
#include <iostream>
#include <jmedia/Error.h>
#include <jmedia/base.h>
#include <jmedia/codec/Decoder.h>
#include <map>
#include <string>
#include <tuple>

using namespace std;
using namespace JMedia;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

namespace JMedia {

class FormatReaderPrivate;
class FormatReader : public Reader {
public:
    FormatReader(const string& filename);
    ~FormatReader();
    int                 open(bool dshow = false);
    int                 close();
    map<string, string> metadata();
    Duration            duration();
    Duration            start_time();
    int                 read_packet(Packet& pkt);
    AVMediaType         media_type(Packet& pkt);
    int                 find_decoder(AVMediaType media_type, Decoder& decoder);
    int getCodecContext(AVMediaType media_type, AVCodecContext*& codecContext);

    int find_stream(AVMediaType media_type, Stream& stream);

private:
    static int FormatReader::interruput(void* arg);

    string               m_filename;
    AVFormatContext*     m_input_format_context;
    std::list<Stream>    m_streams;
    FormatReaderPrivate* priv;
};
};  // namespace JMedia

#endif  // DECODE_AUDIO_JMEDIAREADER_H
