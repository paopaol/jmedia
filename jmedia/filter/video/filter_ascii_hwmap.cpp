#include "filter_ascii_chan.h"


namespace JMedia {
	VideoFilterAsciiChan &VideoFilterAsciiChan::hwmap(const std::string &derive_device)
	{
		char hwmap[1024] = { 0 };

		snprintf(hwmap, 
			sizeof(hwmap) - 1, 
			"hwmap=derive_device=%s", derive_device.c_str());
		m_chan.push_back(hwmap);
		return *this;
	}
}