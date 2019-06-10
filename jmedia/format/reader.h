//
// Created by jz on 17-1-2.
//

#ifndef DECODE_AUDIO_READER_H
#define DECODE_AUDIO_READER_H

#include <map>
#include <string>

#include <jmedia/Error.h>
#include <jmedia/base.h>
#include <jmedia/codec/decoder.h>

namespace JMedia {
struct Duration {
    int Hours;
    int Minutes;
    int Seconds;
    int Us;

    std::string String()
    {
        char buf[64] = { 0 };
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%02d", Hours, Minutes,
                 Seconds, (100 * Us) / AV_TIME_BASE);
        return std::string(buf);
    }
};

class Reader {
public:
    virtual int                                open(bool dshow)         = 0;
    virtual int                                close()                  = 0;
    virtual std::map<std::string, std::string> metadata()               = 0;
    virtual Duration                           duration()               = 0;
    virtual Duration                           start_time()             = 0;
    virtual int                                read_packet(Packet& pkt) = 0;
    virtual AVMediaType                        media_type(Packet& pkt)  = 0;
    virtual int find_decoder(AVMediaType media_type, Decoder& decoder)  = 0;
    virtual int getCodecContext(AVMediaType      media_type,
                                AVCodecContext*& codecContext)          = 0;
    const char* errors()
    {
        return m_error.what();
    }

protected:
    error m_error;
};
}  // namespace JMedia

#endif  // DECODE_AUDIO_READER_H
