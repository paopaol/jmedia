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
#include "jmedia/Filter/FilterGraph.h"
#include "jmedia/Filter/FilterConfig.h"
#include "jmedia/Filter/FilterConfig_abuffer.h"
#include "jmedia/Filter/FilterConfig_abuffersink.h"




static int create_resample_context(JMedia::FilterGraph &graph, AVFrame *decoded_frame)
{
    JMedia::FilterConfig_abuffer src(&graph, "src");
    src.set_sample_rate(decoded_frame->sample_rate);
    src.set_sample_fmt((AVSampleFormat)decoded_frame->format);

    AVRational time = {1, decoded_frame->sample_rate};
    src.set_time_base(time);
    src.set_channel_layout(decoded_frame->channel_layout);
    src.init();
    JMedia::FilterConfig_abuffersink sink(&graph, "sink");
    sink.init();
    src.link(sink);

    graph.config();
    graph.set_src_sink(src, sink);

    return 0;
}

static int resample_audio()
{
    return 0;
}


int main(int argc, char *argv[]) {
    JMedia::Reader      audio("in.mp3");
    int error;



    av_register_all();
    avfilter_register_all();



    JMedia::FilterGraph     graph;




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
                create_resample_context(graph, f);


                error = graph.src_add_frame(f);
                if (error < 0 ){
                    puts(graph.errors().c_str());
                    return 0;
                }

                while ((error = graph.sink_get_frame(f)) >= 0){
                    error = decoder.convert_to_pcm(f, pcm);
                    if (error < 0) {
                        puts(decoder.errors().c_str());
                        return 1;
                    }
                    write(1, pcm.c_str(), pcm.size());
                }
            }

            while(!frames.empty()){
                AVFrame *f = frames.front();
                av_frame_free(&f);
                frames.pop_front();
            }
        }
    }
    return 0;
}