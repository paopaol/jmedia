//
// Created by jz on 16-12-21.
//


#include <unistd.h>

#include <iostream>
#include <string>
using  namespace std;



extern "C" {
#include "libavformat/avformat.h"
}

#include "jmedia/Reader.h"
#include "jmedia/Decoder.h"

int main(int argc, char *argv[]) {
    JMedia::Reader      audio("mp4.mp4");
    int error;


    av_register_all();

    error = audio.open();
    if (error < 0) {
        puts(audio.error().c_str());
        return 1;
    }

    JMedia::Packet  pkt;
    std::list<AVFrame *>       frames;

    while (1) {
        error = audio.read_packet(pkt);
        if (error < 0) {
            puts(audio.error().c_str());
            return 1;
        }
        if (audio.media_type(pkt) == AVMEDIA_TYPE_AUDIO){
            JMedia::Decoder decoder = audio.find_decoder(AVMEDIA_TYPE_AUDIO);

            error = decoder.decode(&pkt.m_pkt, frames);
            if (error < 0) {
                puts(audio.error().c_str());
                return 1;
            }
            for (auto frame = frames.begin(); frame != frames.end(); frame++){
                string pcm;
                AVFrame *f = *frame;
                error = decoder.convert_to_pcm(f, pcm);
                if (error < 0) {
                    puts(decoder.errors().c_str());
                    return 1;
                }
                write(1, pcm.c_str(), pcm.size());
            }

            while(!frames.empty()){
                AVFrame *f = frames.front();
                av_frame_free(&f);
                frames.pop_front();
            }
        }
    }
    av_image_copy();
    return 0;
}