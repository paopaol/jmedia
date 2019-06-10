#include "filter_ascii_chan.h"

namespace JMedia {

	VideoFilterAsciiChan &VideoFilterAsciiChan::scale_qsv(int w, int h)
	{
		char scale[1024] = { 0 };

		snprintf(scale, sizeof(scale) - 1, "scale_qsv=w=%d:h=%d", w, h);
		m_chan.push_back(scale);
		return *this;
	}
}