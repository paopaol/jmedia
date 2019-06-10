#include "filter_ascii_chan.h"

namespace JMedia {
	VideoFilterAsciiChan &VideoFilterAsciiChan::format(const std::string &output_fmt)
	{
		char fmt[1024] = { 0 };

		snprintf(fmt, sizeof(fmt) - 1, "format=%s", output_fmt.c_str());
		m_chan.push_back(fmt);
		return *this;
	}
}