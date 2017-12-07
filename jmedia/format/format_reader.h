//
// Created by jz on 16-12-24.
//

#ifndef DECODE_AUDIO_JMEDIAREADER_H
#define DECODE_AUDIO_JMEDIAREADER_H

#include <iostream>
#include <exception>
#include <string>
#include <tuple>
#include <map>
#include "Decoder.h"
#include "Error.h"
#include "base.h"
#include "Reader.h"

using namespace std;
using namespace JMedia;

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}


namespace JMedia {
    struct Stream{
        AVMediaType         media_type;
        AVCodecContext      *codec_context;
        Decoder             decoder;
        int                 stream_index;
		AVStream			*stream;
    };


    class FormatReader:public Reader{
    public:
        FormatReader(const string &filename);
        ~FormatReader();
        int open();
		int close();
		map<string, string> metadata();
		Duration duration();
		Duration start_time();
        int read_packet(Packet &pkt);
        AVMediaType media_type(Packet &pkt);
        int find_decoder(AVMediaType media_type, Decoder &decoder);
        int getCodecContext(AVMediaType media_type, AVCodecContext *&codecContext);

	private:
		int findStream(AVMediaType media_type, Stream &stream);
    private:
        string                                      m_filename;
        AVFormatContext                             *m_input_format_context;
        std::list<Stream>                           m_streams;
    };
};









#endif //DECODE_AUDIO_JMEDIAREADER_H
