//
// Created by jz on 16-12-21.
//


#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

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



static void save_file(std::vector<uint8_t> pcm)
{
    char    *name = "1.pcm";

    FILE *f = fopen(name, "ab+");

    fwrite(&pcm[0], pcm.size(), 1, f);
    fclose(f);
}

static int create_resample_context(JMedia::FilterGraph &graph, AVFrame *decoded_frame,
    int to_sample_rate,
    AVSampleFormat to_sample_fmt
    )
{
    JMedia::FilterConfig_abuffer src(&graph, "src");
    src.set_sample_rate(decoded_frame->sample_rate);
    src.set_sample_fmt((AVSampleFormat)decoded_frame->format);

    AVRational time = {1, decoded_frame->sample_rate};
    src.set_time_base(time);
    src.set_channel_layout(decoded_frame->channel_layout);
    src.init();

    int a = av_get_channel_layout_nb_channels(decoded_frame->channel_layout);

    JMedia::FilterConfig_abuffersink sink(&graph, "sink");
    sink.set_channel_layout(decoded_frame->channel_layout);
    sink.set_sample_rate(to_sample_rate);
    sink.set_sample_fmt(to_sample_fmt);
    sink.init();
    src.link(sink);

    graph.config();
    graph.set_src_sink(src, sink);

    return 0;
}




int main(int argc, char *argv[]) {
    JMedia::Reader      audio("inin.mkv");
    int error;
    JMedia::FilterGraph     graph;

    av_register_all();
    avfilter_register_all();

    error = audio.open();
    if (error < 0) {
        puts(audio.error().c_str());
        return 1;
    }

    std::list<AVFrame *>        frames;
    bool                        inited = false;

    while (1) {
        JMedia::Packet              pkt;
        error = audio.read_packet(pkt);
        if (error < 0) {
            puts(audio.error().c_str());
            return 1;
        }
        if (audio.media_type(pkt) == AVMEDIA_TYPE_AUDIO){
            JMedia::Decoder &decoder = audio.find_decoder(AVMEDIA_TYPE_AUDIO);
            error = decoder.decode(&pkt.m_pkt, frames);
            if (error < 0) {
                puts(decoder.errors().c_str());
                return 1;
            }
            for (auto frame = frames.begin(); frame != frames.end(); frame++){
                AVFrame *f = *frame;
                if (!inited){
                    create_resample_context(graph, f, 44100, AV_SAMPLE_FMT_S16);
                    inited = true;
                }


                error = graph.src_add_frame(f);
                if (error < 0 ){
                    puts(graph.errors().c_str());
                    return 0;
                }

                while ((error = graph.sink_get_frame(f)) >= 0){
                    std::vector<uint8_t> pcm;
                    error = decoder.convert_to_pcm(f, pcm);
                    if (error < 0) {
                        puts(decoder.errors().c_str());
                        return 1;
                    }
                    save_file(pcm);
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