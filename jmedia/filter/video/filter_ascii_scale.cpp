#include "filter_ascii_chan.h"

namespace JMedia {
	VideoFilterAsciiChan& VideoFilterAsciiChan::scale(int w, int h)
	{
		//scale = w = 200:h = 100
		char scale[256] = { 0 };
		sprintf(scale, "scale=w=%d:h=%d", w, h);
		m_chan.push_back(scale);
		return *this;
	}

	VideoFilterAsciiChan &VideoFilterAsciiChan::zoomX(int xx, int yx)
	{
		//scale=w=2*iw:h=2*ih
		char scale[256] = { 0 };
		sprintf((char *)scale, "scale=w=%d*iw:h=%d*ih", xx, yx);
		m_chan.push_back(scale);
		return *this;
	}
	VideoFilterAsciiChan &VideoFilterAsciiChan::reduceX(int xx, int yx)
	{
		//scale=w=iw/2:h=ih/2
		char scale[256] = { 0 };
		sprintf((char *)scale, "scale=w=iw/%d:h=ih/%d", xx, yx);
		m_chan.push_back(scale);
		return *this;
	}
}