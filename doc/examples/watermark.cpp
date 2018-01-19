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
#include <filter/filter_graph.h>
#include <filter/filter_buffer.h>
#include <filter/filter_buffersink.h>
#include <filter/filter_graph.h>
#include <filter/video/filter_ascii_graph.h>
#include <filter/video/filter_ascii_chan.h>

using  namespace std;
using namespace std::tr2::sys;

#pragma pack(push, 1)
typedef struct tagBitmapFileHeader
{
	unsigned short  bfType; //2 λͼ�ļ������ͣ�����Ϊ��BM��    
	unsigned long bfSize; //4 λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ    
	unsigned short bfReserved1; //2 λͼ�ļ������֣�����Ϊ0    
	unsigned short bfReserved2; //2 λͼ�ļ������֣�����Ϊ0    
	unsigned long bfOffBits; //4 λͼ���ݵ���ʼλ�ã��������λͼ�ļ�ͷ��ƫ������ʾ�����ֽ�Ϊ��λ    
} BitmapFileHeader;//�ýṹռ��14���ֽڡ�    
#pragma pack(pop)

typedef struct tagBitmapInfoHeader {
	unsigned long biSize; //4 ���ṹ��ռ���ֽ���    
	long biWidth; //4 λͼ�Ŀ�ȣ�������Ϊ��λ    
	long biHeight; //4 λͼ�ĸ߶ȣ�������Ϊ��λ    
	unsigned short biPlanes; //2 Ŀ���豸��ƽ�������壬����Ϊ1    
	unsigned short biBitCount;//2 ÿ�����������λ����������1(˫ɫ), 4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ    
	unsigned long biCompression; //4 λͼѹ�����ͣ������� 0(��ѹ��),1(BI_RLE8ѹ������)��2(BI_RLE4ѹ������)֮һ    
	unsigned long biSizeImage; //4 λͼ�Ĵ�С�����ֽ�Ϊ��λ    
	long biXPelsPerMeter; //4 λͼˮƽ�ֱ��ʣ�ÿ��������    
	long biYPelsPerMeter; //4 λͼ��ֱ�ֱ��ʣ�ÿ��������    
	unsigned long biClrUsed;//4 λͼʵ��ʹ�õ���ɫ���е���ɫ��    
	unsigned long biClrImportant;//4 λͼ��ʾ��������Ҫ����ɫ��    
} BitmapInfoHeader;//�ýṹռ��40���ֽڡ�





static int save_bmp(string &fname, AVFrame *f)
{

	BitmapFileHeader bmpHeader;
	BitmapInfoHeader bmpInfo;
 
    FILE            *fp = NULL;
    char            outputfile[1024] = {0};
    static int      index = 0;

	path filePath(fname);

    sprintf(outputfile, "%s_%d_%d_%d.bmp", filePath.stem().string().c_str(), f->width, f->height, index);

	std::string file = filePath.parent_path().append(outputfile).string();

    fp = fopen(file.c_str(),"wb");

	bmpHeader.bfType = 0x4d42;
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;
	bmpHeader.bfOffBits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
	bmpHeader.bfSize = bmpHeader.bfOffBits + f->width * f->height * 3;

	bmpInfo.biSize = sizeof(BitmapInfoHeader);
	bmpInfo.biWidth = f->width;
	bmpInfo.biHeight = f->height;
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 24;
	bmpInfo.biCompression = 0;
	bmpInfo.biSizeImage = 0;
	bmpInfo.biXPelsPerMeter = 100;
	bmpInfo.biYPelsPerMeter = 100;
	bmpInfo.biClrUsed = 0;
	bmpInfo.biClrImportant = 0;

	fwrite(&bmpHeader, sizeof(bmpHeader), 1, fp);
	fwrite(&bmpInfo, sizeof(bmpInfo), 1, fp);
	fwrite(f->data[0], f->width * f->height * 3, 1, fp);
	fclose(fp);
	index++;
	return 0;
}

static void save_yuv_file(string &fname, AVFrame *f)
{
    FILE            *fp = NULL;
    char            outputfile[1024] = {0};
    static int      index = 0;

	path filePath(fname);

    sprintf(outputfile, "%s_%d_%d.yuv", filePath.stem().string().c_str(), f->width, f->height);

	std::string file = filePath.parent_path().append(outputfile).string();

    fp = fopen(file.c_str(),"ab+");
	int s = f->height * f->width;
	fwrite(f->data[0], 1, f->height * f->width, fp);    //Y 
	fwrite(f->data[1], 1, f->height * f->width / 4, fp);  //U
	fwrite(f->data[2], 1, f->height * f->width/ 4, fp);  //V


    fclose(fp);
    index++;
}


static AVPixelFormat ajust_pix_fmt(AVPixelFormat deprecated_fmt)
{
	switch (deprecated_fmt) {
	case AV_PIX_FMT_YUVJ420P:
		return AV_PIX_FMT_YUV420P;
	case AV_PIX_FMT_YUVJ422P:
		return AV_PIX_FMT_YUV422P;
	case AV_PIX_FMT_YUVJ444P:
		return AV_PIX_FMT_YUV444P;
	case AV_PIX_FMT_YUVJ440P:
		return AV_PIX_FMT_YUV440P;
	default:
		return deprecated_fmt;
	}
}


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
	puts("video_watermask inputfile logofile");
}


int main(int argc, char *argv[]) {
	if (argc != 3) {
		Usage();
		return 1;
	}
	std::string filename = argv[1];
	std::string logo = argv[2];



    av_register_all();
	avfilter_register_all();
    avformat_network_init();

    int error;
    JMedia::FormatReader        file(argv[1]);
	JMedia::FilterGraph         graph;
	JMedia::FilterBuffer	    buffer(&graph, "in");
	JMedia::FilterBuffersink    buffersink(&graph, "out");

	graph.set_src_sink(buffer, buffersink);


    error = file.open();
    if (error < 0) {
        puts(file.errors());
        return 1;
    }
	std::shared_ptr<void> fileClose(nullptr, std::bind(&FormatReader::close, &file));

	AVCodecContext *codecContext = NULL;
	error = file.getCodecContext(AVMEDIA_TYPE_VIDEO, codecContext);
	if (error < 0) {
		puts(file.errors());
		return 1;
	}


	//h w
	buffer.set_h(codecContext->height);
	buffer.set_w(codecContext->width);

	//pix_fmt
	AVPixelFormat pix_fmt = ajust_pix_fmt((AVPixelFormat)codecContext->pix_fmt);
	buffer.set_pix_fmt(pix_fmt);


	//time_base
	AVRational time_base = codecContext->time_base;
	time_base.num = time_base.num == 0 ? 1 : time_base.num;
	buffer.set_time_base(time_base);

	//sample aspect ratio
	AVRational sample_aspect_ratio = codecContext->sample_aspect_ratio;
	sample_aspect_ratio.num = sample_aspect_ratio.num == 0 ? 1 : sample_aspect_ratio.num;
	buffer.set_pixel_aspect(sample_aspect_ratio);

	//init all set of paramters
	error = buffer.init_str();
	if (error < 0) {
		puts(buffer.errors());
		return 1;
	}

	//set output pix_fmts
	std::vector<AVPixelFormat> pix_fmts = {
		AV_PIX_FMT_YUV420P,
		AV_PIX_FMT_NONE
	};
	buffersink.set_pix_fmts(pix_fmts);

	error = buffersink.init_str();
	if (error < 0) {
		puts(buffer.errors());
		return 1;
	}

	//auto escape_moive = [](const std::string &logo) {
	//	return "\\'" + logo + "\\'";
	////};

	JMedia::VideoFilterAsciiGraph g;
	JMedia::VideoFilterAsciiChan  ch;

	g.push_chan(
		ch.reset().movie(logo).output("wm")
	).push_chan(
		ch.reset().in("wm").scale(100, 100).output("smallwm")
	).push_chan(
		ch.reset().in("in").in("smallwm").overlay(5, 5).output("out")
	);

	//logo = escape_moive(logo);
	//char filterdescri[1024] = { 0 };
	//sprintf(filterdescri, "movie=%s[wm];[in][wm]overlay=5:5[out]", logo.c_str());
	//sprintf(filterdescri, "movie=%s[wm];[in][wm]overlay=5:5,scale=iw/5:ih/5[out]", logo.c_str());
	error = graph.config(g.string());
	if (error < 0) {
		puts(graph.errors());
		return 1;
	}

	std::list<AVFrame *>        frames;
	JMedia::Packet              pkt;
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
			for (auto frame = frames.begin(); frame != frames.end(); frame++) {
				AVFrame *f = *frame;

				f->pts = av_frame_get_best_effort_timestamp(f);

				error = graph.src_add_frame(f);
				if (error < 0) {
					puts(graph.errors());
					return 1;
				}

				std::list<AVFrame *> gotFrameList;
				error = graph.sink_get_frame(gotFrameList);
				if (error < 0) {
					puts(graph.errors());
					return 1;
				}
				if (gotFrameList.size() == 0) {
					continue;
				}
				std::shared_ptr<void> deferFreeGotFrames(nullptr, std::bind(freeAVFrames, &gotFrameList));

				for(auto &frame : gotFrameList) {
					save_yuv_file(filename, frame);
					//save_bmp(filename, frame);
				}
			}

		}
    }
    return 0;
}