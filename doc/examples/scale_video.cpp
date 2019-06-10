
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
#include <scaler/scaler.h>

#include <Windows.h>

using  namespace std;
using namespace std::tr2::sys;

#pragma pack(push, 1)
typedef struct tagBitmapFileHeader {
    unsigned short  bfType; //2 位图文件的类型，必须为“BM”
    unsigned long bfSize; //4 位图文件的大小，以字节为单位
    unsigned short bfReserved1; //2 位图文件保留字，必须为0
    unsigned short bfReserved2; //2 位图文件保留字，必须为0
    unsigned long
    bfOffBits; //4 位图数据的起始位置，以相对于位图文件头的偏移量表示，以字节为单位
} BitmapFileHeader;//该结构占据14个字节。
#pragma pack(pop)

typedef struct tagBitmapInfoHeader {
    unsigned long biSize; //4 本结构所占用字节数
    long biWidth; //4 位图的宽度，以像素为单位
    long biHeight; //4 位图的高度，以像素为单位
    unsigned short biPlanes; //2 目标设备的平面数不清，必须为1
    unsigned short
    biBitCount;//2 每个像素所需的位数，必须是1(双色), 4(16色)，8(256色)或24(真彩色)之一
    unsigned long
    biCompression; //4 位图压缩类型，必须是 0(不压缩),1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
    unsigned long biSizeImage; //4 位图的大小，以字节为单位
    long biXPelsPerMeter; //4 位图水平分辨率，每米像素数
    long biYPelsPerMeter; //4 位图垂直分辨率，每米像素数
    unsigned long biClrUsed;//4 位图实际使用的颜色表中的颜色数
    unsigned long biClrImportant;//4 位图显示过程中重要的颜色数
} BitmapInfoHeader;//该结构占据40个字节。


void SaveBGR32ToBMP(BYTE* rgbaData, int rgbaSize, int width, int height,
                    char* bmpFile)
{
    FILE* fp = NULL;
    BITMAPFILEHEADER kFileHeader;
    BITMAPINFOHEADER kInfoHeader;
    memset(&kFileHeader, 0, sizeof(BITMAPFILEHEADER));
    memset(&kInfoHeader, 0, sizeof(BITMAPINFOHEADER));

    kFileHeader.bfType = 0x4d42;
    kFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                         rgbaSize;
    kFileHeader.bfReserved1 = 0;
    kFileHeader.bfReserved2 = 0;
    kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    kInfoHeader.biWidth = width;
    kInfoHeader.biHeight = height;
    kInfoHeader.biPlanes = 1;
    kInfoHeader.biBitCount = 32;
    kInfoHeader.biCompression = 0L;
    kInfoHeader.biSizeImage = 0;
    kInfoHeader.biXPelsPerMeter = 0;
    kInfoHeader.biYPelsPerMeter = 0;
    kInfoHeader.biClrUsed = 0;
    kInfoHeader.biClrImportant = 0;

    fp = fopen(bmpFile, "wb+");
    if (!fp) {
        return;
    }

    fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);


    fwrite(rgbaData, 1, rgbaSize, fp);
    fclose(fp);
}


static int save_bmp(string& fname, AVFrame* f)
{

    BitmapFileHeader bmpHeader;
    BitmapInfoHeader bmpInfo;

    FILE*            fp = NULL;
    char            outputfile[1024] = {0};
    static int      index = 0;

    path filePath("123");

    sprintf(outputfile, "%s_%d_%d_%d.bmp", filePath.stem().string().c_str(),
            f->width, f->height, index);

    std::string file = filePath.parent_path().append(outputfile).string();

    fp = fopen(file.c_str(), "wb");

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

static void save_yuv_file(string& fname, AVFrame* f)
{
    FILE*            fp = NULL;
    char            outputfile[1024] = {0};
    static int      index = 0;

    path filePath("111");

    sprintf(outputfile, "%s_%d_%d.yuv", filePath.stem().string().c_str(), f->width,
            f->height);

    std::string file = filePath.parent_path().append(outputfile).string();

    fp = fopen(file.c_str(), "ab+");
    int s = f->height * f->width;
    fwrite(f->data[0], 1, f->height * f->width, fp);    //Y
    fwrite(f->data[1], 1, f->height * f->width / 4, fp);  //U
    fwrite(f->data[2], 1, f->height * f->width / 4, fp); //V


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


static void freeAVFrames(std::list<AVFrame*>* frames)
{
    while (!frames->empty()) {
        AVFrame* f = frames->front();
        //av_frame_unref(f);
        av_frame_free(&f);
        frames->pop_front();
    }
}




static void Usage()
{
    puts("scale_video inputfile w h [yuv/bmp]");
}


int main(int argc, char* argv[])
{
    if (argc != 5) {
        Usage();
        return 1;
    }
    std::string filename = argv[1];
    int w = atoi(argv[2]);
    int h = atoi(argv[3]);
    std::map<std::string, AVPixelFormat> outfmt = {
        {"yuv", AV_PIX_FMT_YUV420P},
        {"bmp", AV_PIX_FMT_RGB24}
    };
    std::string pix_fmt = argv[4];
    if (outfmt.find(pix_fmt) == outfmt.end()) {
        Usage();
        return 1;
    }

    av_register_all();
    avformat_network_init();

    int error;
    JMedia::FormatReader        file(argv[1]);

    JMedia::Scaler scaler;
    JMedia::ScalerConfig conf;


    error = file.open(true);
    if (error < 0) {
        puts(file.errors());
        return 1;
    }
    std::shared_ptr<void> fileClose(nullptr, std::bind(&FormatReader::close,
                                    &file));

    std::list<AVFrame*>        frames;
    JMedia::Packet              pkt;

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
        //defer unref pkt
        std::shared_ptr<void> unrefPkt(nullptr, std::bind(&JMedia::Packet::unref,
                                       &pkt));

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
        std::shared_ptr<void> deferFreeFrames(nullptr, std::bind(freeAVFrames,
                                              &frames));


        if (mediaType == AVMEDIA_TYPE_VIDEO) {
            for (auto frame = frames.begin(); frame != frames.end(); frame++) {
                AVFrame* f = *frame;


                conf.src_height = f->height;
                conf.src_width = f->width;
                conf.src_pix_fmt = ajust_pix_fmt((AVPixelFormat)f->format);

                conf.dst_height = h;
                conf.dst_width = w;
                conf.dst_pix_fmt = AV_PIX_FMT_YUV420P;

                error = scaler.init_once(conf);
                if (error < 0) {
                    puts(scaler.errors());
                    return 1;
                }
                AVFrame* rgbFrame = NULL;
                error = scaler.scale(f, rgbFrame);
                if (error < 0) {
                    puts(scaler.errors());
                    return 1;
                }
                rgbFrame = scaler.get_scaled();

#if 1
                if (conf.dst_pix_fmt == AV_PIX_FMT_YUV420P) {
                    save_yuv_file(filename, rgbFrame);
                } else if(conf.dst_pix_fmt == AV_PIX_FMT_BGRA) {
                    SaveBGR32ToBMP(rgbFrame->data[0], rgbFrame->width * rgbFrame->height * 4,
                                   rgbFrame->width, rgbFrame->height, "bgra.bmp");

                } else {
                    // save_bmp(filename, rgbFrame);
                    SaveBGR32ToBMP(rgbFrame->data[0], rgbFrame->width * rgbFrame->height * 4,
                                   rgbFrame->width, rgbFrame->height, "bgra.bmp");
                }
#endif
            }

        }
    }
    return 0;
}
