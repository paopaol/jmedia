//
// Created by jz on 16-12-21.
//

#include <stdio.h>

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <filesystem>

#include <format_reader.h>

using  namespace std;
using namespace std::tr2::sys;


static void freeAVFrames(std::list<AVFrame *> *frames)
{
	while (!frames->empty()) {
		AVFrame *f = frames->front();
		av_frame_unref(f);
		av_frame_free(&f);
		frames->pop_front();
	}
}


static void Usage()
{
	puts("decode_media inputfile");
}


int main(int argc, char *argv[]) {
	if (argc != 2) {
		Usage();
		return 1;
	}
	std::string filename = argv[1];

    av_register_all();
    avformat_network_init();

    int error;
    JMedia::FormatReader        file(argv[1]);

    error = file.open();
    if (error < 0) {
        puts(file.errors());
        return 1;
    }
	std::shared_ptr<void> fileClose(nullptr, std::bind(&FormatReader::close, &file));

	std::list<AVFrame *>        frames;
	JMedia::Packet              pkt;

	//init pkt
	pkt.init();
    while (1) {
        error = file.read_packet(pkt);
        if (error == AVERROR_EOF){
            break;
        }
        if (error < 0) {
            puts(file.errors());
            return 1;
        }
		//defer unref pkt
		std::shared_ptr<void> unrefPkt(nullptr, std::bind(&JMedia::Packet::unref, &pkt));

		AVMediaType mediaType = file.media_type(pkt);
		JMedia::Decoder decoder;
		error = file.find_decoder(mediaType, decoder);
		if (error < 0) {
			puts(file.errors());
			return 1;
		}
		error = decoder.decode(&pkt.m_pkt, frames);
		if (error < 0) {
			puts(decoder.errors());
			return 1;
		}
		if (frames.size() == 0) {
			continue;
		}
		std::shared_ptr<void> deferFreeFrames(nullptr, std::bind(freeAVFrames, &frames));

		if (mediaType == AVMEDIA_TYPE_VIDEO) {
			//got video decoded frame list
			for (auto vframe = frames.begin(); vframe != frames.end(); vframe++) {
				AVFrame *f = *vframe;
				//do something for video
			}
		}
		else if (mediaType == AVMEDIA_TYPE_AUDIO) {
			//got audio decoded frame list
			for (auto aframe = frames.begin(); aframe != frames.end(); aframe++) {
				AVFrame *f = *aframe;
				//do something for audio
			}
		}
    }
    return 0;
}
