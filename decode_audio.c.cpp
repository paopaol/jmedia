//
// Created by jz on 16-12-21.
//

#include <stdio.h>

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include <string>
using  namespace std;



extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
}

#include "jmedia/FormatReader.h"
#include "jmedia/Filter/FilterGraph.h"
#include "jmedia/Filter/FilterConfig_abuffer.h"
#include "jmedia/Filter/FilterConfig_aformat.h"
#include "jmedia/Filter/FilterConfig_abuffersink.h"
#include "jmedia/tools/Resampler.h"
#include "jmedia/tools/Scaler.h"

static void string_replace(string &s1,const string &s2,const string &s3)
{
    string::size_type pos=0;
    string::size_type a=s2.size();
    string::size_type b=s3.size();
    while((pos=s1.find(s2,pos))!=string::npos)
    {
        s1.replace(pos,a,s3);
        pos+=b;
    }
}

static int save_video_file_rgb24(string &fname, uint8_t *data, int size, AVFrame *f)
{
    FILE            *fp = NULL;
    string          file = fname;
    char            buf[1024] = {0};
    static int      index = 0;

    string_replace(file, "/", "_");
    sprintf(buf, "%s_%d.pgm", file.c_str(), index);

    fp = fopen(buf,"ab+");
    fprintf(fp, "P5\n%d %d\n%d\n", f->width, f->height, 255);
    fwrite(data, 1, size, fp);
    fclose(fp);
    index++;

    return 0;
}

static void save_video_file(string &fname, AVFrame *f)
{
    FILE            *fp = NULL;
    string          file = fname;
    char            buf[1024] = {0};
    static int      index = 0;

    string_replace(file, "/", "_");
    sprintf(buf, "%s_%d.pgm", file.c_str(), index);

    fp = fopen(buf,"ab+");
    fprintf(fp, "P5\n%d %d\n%d\n", f->width, f->height, 255);
    for (int i = 0; i < f->height; i++){
        fwrite(f->data[0] + i * f->linesize[0], 1, f->width, fp);
    }
    fclose(fp);
    index++;
}



static void save_file(string &fname, std::vector<uint8_t> pcm, int channels, uint64_t channel_layout, int fmt, int sample_rate)
{
    char            name[1024] = {0};
    char            channel_layout_name[64] = {0};
    string          file = fname;

    string_replace(file, "/", "_");

    av_get_channel_layout_string(channel_layout_name, sizeof(channel_layout_name), channels, channel_layout);

    sprintf(name, "%s_%d_%d_%s_%d_%s.pcm",
             channel_layout_name,
             channels,
             sample_rate,
             av_get_sample_fmt_name((AVSampleFormat)fmt),
             av_get_bytes_per_sample((AVSampleFormat)fmt),
             file.c_str());

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


    JMedia::FilterConfig_aformat aformat(&graph, "aformat");
    av_get_default_channel_layout(decoded_frame->channels);
    aformat.set_channel_layout_s(av_get_default_channel_layout(decoded_frame->channels));
    aformat.set_sample_rate_s(to_sample_rate);
    aformat.set_sample_fmt_s(to_sample_fmt);
    aformat.init();

    JMedia::FilterConfig_abuffersink sink(&graph, "sink");
    sink.init();

    try{
        src.link(aformat).link(sink);
    }catch (JMedia::Error &e){
        puts(e.what());
        return -1;
    }

    graph.config();
    graph.set_src_sink(src, sink);

    return 0;
}











int main(int argc, char *argv[]) {
//    string filename = "rtmp://live.hkstv.hk.lxdns.com/live/hks";
//    string filename = argv[1];
    string filename = "dll.mkv";
    JMedia::FormatReader        audio(filename);
    int error;
    JMedia::Resampler           resampler;
    JMedia::Scaler              scaler;

    av_register_all();
    avformat_network_init();

    error = audio.open();
    if (error < 0) {
        puts(audio.errors());
        return 1;
    }

    std::list<AVFrame *>        frames;


    while (1) {
        JMedia::Packet              pkt;
        error = audio.read_packet(pkt);
        if (error == AVERROR_EOF){
            break;
        }
        if (error < 0) {
            puts(audio.errors());
            return 1;
        }

        JMedia::Decoder &decoder = audio.find_decoder(audio.media_type(pkt));
        switch (audio.media_type(pkt)){
            case AVMEDIA_TYPE_AUDIO:
                error = decoder.decode(&pkt.m_pkt, frames);
                if (error < 0) {
                    puts(decoder.errors().c_str());
                    return 1;
                }
                for (auto frame = frames.begin(); frame != frames.end(); frame++){
                    AVFrame *f = *frame;

//                    JMedia::ResampleConfig config;
//
//                    config.dst_ch_layout = AV_CH_LAYOUT_6POINT1;
//                    config.dst_rate = 96000;
//                    config.dst_sample_fmt = AV_SAMPLE_FMT_U8;
//
//                    config.src_ch_layout = f->channel_layout;
//                    config.src_rate = f->sample_rate;
//                    config.src_sample_fmt = (AVSampleFormat)f->format;
//
//                    resampler.init_once(config);
//
//                    error = resampler.convert((const uint8_t **)f->extended_data, f->nb_samples);
//                    if (error < 0){
//                        printf("error\n");
//                        return 1;
//                    }
//
                    std::vector<uint8_t> pcm_new;
//                    uint8_t     *data;
//                    int         size;
//
//                    resampler.get_converted(data, size);
//                    std::copy(data, data + size, std::back_inserter(pcm_new));
                    decoder.convert_to_pcm(f, pcm_new);
                    save_file(filename, pcm_new, f->channels, f->channel_layout, f->format, f->sample_rate);
                }
                break;
            case AVMEDIA_TYPE_VIDEO:
                error = decoder.decode(&pkt.m_pkt, frames);
                if (error < 0){
                    puts(decoder.errors().c_str());
                    return 1;
                }
                for (auto frame = frames.begin(); frame != frames.end(); frame++){
                    AVFrame *f = *frame;
                    JMedia::ScalerConfig    scaler_config;

                    scaler_config.src_width = f->width;
                    scaler_config.src_height = f->height;
                    scaler_config.src_pix_fmt = (AVPixelFormat)f->format;

                    scaler_config.dst_width = f->width;
                    scaler_config.dst_height = f->height;
                    scaler_config.dst_pix_fmt = AV_PIX_FMT_BGR24;

                    error = scaler.init_once(scaler_config);
                    if (error < 0){
                        printf("s error\n");
                        return 1;
                    }

                    error = scaler.convert((const uint8_t **)f->extended_data, f->linesize);
                    if (error < 0){
                        printf("sc error\n");
                        return 2;
                    }



                    uint8_t     *data;
                    int         size;

                    scaler.get_converted(data, size);


                    save_video_file_rgb24(filename, data, size, f);
                    //save_video_file(filename, f);
                }
                break;
            default:
                break;
        }



        while(!frames.empty()){
            AVFrame *f = frames.front();
            av_frame_free(&f);
            frames.pop_front();
        }
    }
    return 0;
}