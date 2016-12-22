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

#include "LibAVInputFileAudio.h"

int main(int argc, char *argv[]) {
    LibAVInputFileAudio audio("in.mp3");
    int error;

    av_register_all();

    error = audio.open();
    if (error < 0) {
        puts(audio.error().c_str());
        return 1;
    }


    while (1) {
        string pcm("", 0);
        error = audio.read_pcm(pcm);
        if (error < 0) {
            puts(audio.error().c_str());
            return 1;
        }
        write(1, pcm.c_str(), pcm.size());
    }
    return 0;
}