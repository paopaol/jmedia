#include "filter_ascii_chan.h"

namespace JMedia {
	VideoFilterAsciiChan &VideoFilterAsciiChan::crop(int x, int y,
		int w, int h)
	{
		//crop=100:100:0:0-w:h:x:y
		char crop[256] = { 0 };
		sprintf(crop, "crop=%d"
			":%d"
			":%d"
			":%d",
			w, h, x, y);
		m_chan.push_back(crop);
		return *this;
	}
}