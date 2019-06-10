#include <stdio.h>

#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <codec/encoder.h>
#include <format/format_writer.h>
#include <format_reader.h>
#include <resampler/resampler.h>

using namespace std;
using namespace std::tr2::sys;

static void freeAVFrames(std::list<AVFrame*>* frames)
{
    while (!frames->empty()) {
        AVFrame* f = frames->front();
        av_frame_unref(f);
        av_frame_free(&f);
        frames->pop_front();
    }
}

static std::list<AVFrame *> videoList;
static int64_t v_next_pts = 0;
static std::list<AVFrame *> audioList;
static int64_t a_next_pts = 0;

static int writeVideo()
{

}


static int resampleAudio(JMedia::Resampler &resampler, AVFrame *in, AVFrame *out)
{
	JMedia::ResamplerConfig conf;

	conf.src_rate = in->sample_rate;
	conf.src_sample_fmt = (AVSampleFormat)in->format;
	conf.src_ch_layout = in->channel_layout;

	conf.dst_rate = 48000;
	conf.dst_sample_fmt = AV_SAMPLE_FMT_FLTP;
	conf.dst_ch_layout = in->channel_layout;

	int error = resampler.init_once(conf);
	if (error < 0) {
		return error;
	}
	error = resampler.convert(in, out);
	if (error < 0) {
		return error;
	}
	return 0;
}

static int encodeAudio(std::shared_ptr<JMedia::Encoder> &encoder)
{
	encoder->set_audio_attr(40000, AV_SAMPLE_FMT_FLTP,
		conf.dst_rate, conf.dst_ch_layout);
	int error = encoder->open_once();
	if (error < 0) {
		return error;
	}
	std::list<AVPacket*> pktList;
	error = aencoder->encode(out, pktList);
	if (error < 0) {
		return error;
	}
}



static int writeAudio()
{
	int error = 0;

	auto f = audioList.front();
	audioList.pop_front();

	JMedia::Resampler       resampler;

	AVFrame *out = nullptr;
	error = resampleAudio(resampler, f, out);
	if (error < 0) {
		return error;
	}


	
}








static void Usage()
{
    puts("resample inputfile");
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        Usage();
        return 1;
    }
    std::string filename = argv[1];

    av_register_all();
    avformat_network_init();

    int                                   error;
    JMedia::FormatReader                  file(argv[1]);
    std::shared_ptr<JMedia::FormatWriter> aac =
        std::make_shared<JMedia::FormatWriter>("123.mp4");

    error = file.open();
    if (error < 0) {
        puts(file.errors());
        return 1;
    }
    std::shared_ptr<void> fileClose(nullptr,
                                    std::bind(&FormatReader::close, &file));

    std::list<AVFrame*> frames;
    JMedia::Packet      pkt;

    // init pkt
    std::shared_ptr<JMedia::Encoder> aencoder;
	std::shared_ptr<JMedia::Encoder> vencoder;
    pkt.init();
    while (1) {
        error = file.read_packet(pkt);
        if (error == AVERROR_EOF) {
            break;
        }
        if (error < 0) {
            puts(file.errors());
            return 1;
        }
        // defer unref pkt
        std::shared_ptr<void> unrefPkt(nullptr,
                                       std::bind(&JMedia::Packet::unref, &pkt));

        AVMediaType     mediaType = file.media_type(pkt);
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
        std::shared_ptr<void> deferFreeFrames(nullptr,
                                              std::bind(freeAVFrames, &frames));

        if (mediaType == AVMEDIA_TYPE_AUDIO) {
            // got audio decoded frame list
            for (auto aframe = frames.begin(); aframe != frames.end();
                 aframe++) {
				AVFrame*                f = *aframe;
				videoList.push_back(f);



                
                
                if (pktList.size() == 0) {
                    continue;
                }
                static int            count = 0;
                std::shared_ptr<void> freepkt(nullptr, std::bind([&]() {
                                                  while (pktList.size()) {
                                                      auto pkt =
                                                          pktList.front();
                                                      av_packet_free(&pkt);
                                                      pktList.pop_front();
                                                  }
                                              }));
                for (auto pkt = pktList.begin(); pkt != pktList.end(); pkt++) {
                    aac->set_audio_attr(AV_SAMPLE_FMT_FLTP, 40000,
                                        conf.dst_rate, conf.dst_ch_layout);
                    error = aac->open_once();
                    if (error < 0) {
                        fprintf(stderr, "%s\n", file.errors());
                        return 1;
                    }
                    aac->write(AVMEDIA_TYPE_AUDIO, *pkt);
                }
            }
        }
        else if (mediaType == AVMEDIA_TYPE_VIDEO) {
            // got audio decoded frame list
            for (auto vframe = frames.begin(); vframe != frames.end();
                 vframe++) {
                AVFrame* f = *vframe;

                if (!vencoder) {
                    vencoder =
                        std::make_shared<JMedia::Encoder>(AV_CODEC_ID_H264);
                }

                vencoder->set_video_attr(40000, ( AVPixelFormat )f->format,
                                        f->width, f->height, { 1, 25 },
                                        { 1, 25 }, 12, 1);
                error = vencoder->open_once();
                if (error < 0) {
                    fprintf(stderr, "%s\n", file.errors());
                    return 1;
                }
                std::list<AVPacket*> pktList;
                error = vencoder->encode(f, pktList);
                if (error < 0) {
                    fprintf(stderr, "%s\n", file.errors());
                    return 1;
                }
                if (pktList.size() == 0) {
                    continue;
                }
                static int            count = 0;
                std::shared_ptr<void> freepkt(nullptr, std::bind([&]() {
                                                  while (pktList.size()) {
                                                      auto pkt =
                                                          pktList.front();
                                                      av_packet_free(&pkt);
                                                      pktList.pop_front();
                                                  }
                                              }));
                for (auto pkt = pktList.begin(); pkt != pktList.end(); pkt++) {
                    aac->set_video_attr(f->width, f->height, { 1, 25 }, 40000,
                                        12, ( AVPixelFormat )f->format);
                    error = aac->open_once();
                    if (error < 0) {
                        fprintf(stderr, "%s\n", file.errors());
                        return 1;
                    }
                    error = aac->write(AVMEDIA_TYPE_VIDEO, *pkt);
					if (error < 0) {
						fprintf(stderr, "%s\n", aac->errors());
						goto over;
					}
                }
            }
        }

		if (av_compare_ts() <= 0) {
			writeVideo();
		}
		else {
			writeAudio();
		}


    }

over:
    aac->close();
}
