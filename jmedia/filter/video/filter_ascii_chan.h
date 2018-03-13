#ifndef FILTER_ASCII_CHAN_H
#define FILTER_ASCII_CHAN_H

#include <vector>
#include <string>

namespace JMedia {
	class VideoFilterAsciiChan {
	public:
		//http://ffmpeg.org/ffmpeg-filters.html#scale-1
		VideoFilterAsciiChan &scale(int w, int h);
		VideoFilterAsciiChan &zoomX(int xx,int yx);
		VideoFilterAsciiChan &reduceX(int xx, int yx);

		//http://ffmpeg.org/ffmpeg-filters.html#movie-1
		VideoFilterAsciiChan &movie(const std::string &path);

		//http://ffmpeg.org/ffmpeg-filters.html#overlay-1
		VideoFilterAsciiChan &overlay(int x = 0, int y = 0);
		VideoFilterAsciiChan &overlay_top_left(int top, int left);
		VideoFilterAsciiChan &overlay_top_right(int top, int right);
		VideoFilterAsciiChan &overlay_bottom_left(int bottom, int left);
		VideoFilterAsciiChan &overlay_bottom_right(int bottom, int right);

        //http://ffmpeg.org/ffmpeg-filters.html#drawbox-1
        VideoFilterAsciiChan &drawbox(int x = 0, int y = 0, //start point
                                    int w = 0, int h = 0, 
                                    int thickness = 3, double opacity = 1.0, 
                                    unsigned int color = 0xFFFFFF,
                                    bool invert = false);

		VideoFilterAsciiChan &crop(int x = 0, int y = 0, int w = 0, int h = 0);

		VideoFilterAsciiChan &drawtext(int x, int y, std::string stitle, int size=70,
			std::string fontfile = "simhei.ttf",std::string scolor="red");

		std::string string();
		VideoFilterAsciiChan &reset();
		VideoFilterAsciiChan &in(const std::string &name);
		VideoFilterAsciiChan &output(const std::string &name);

	private:
		std::string prepend_in();
		std::string append_out();

	private:
		std::vector<std::string> m_chan;
		std::vector<std::string> m_in;
		std::vector<std::string> m_out;
	};
}

#endif