#include "filter_ascii_chan.h"

namespace JMedia {
	VideoFilterAsciiChan &VideoFilterAsciiChan::drawtext(int x, int y, std::string stitle, int size, std::string fontfile, std::string scolor)
	{
		//drawtext=fontfile=simhei.ttf:fontcolor=red:fontsize=70:text='title':x=1000:y=1000
		char drawtext[256] = { 0 };
		sprintf(drawtext, "drawtext=x=%d:"
			"y=%d:"
			"fontfile=%s:"
			"fontcolor=%s:"
			"fontsize=%d:"
			"text=\'%s\'",
			x, y, fontfile.c_str(), scolor.c_str(), size, stitle.c_str());
		m_chan.push_back(drawtext);
		return *this;
	}
}