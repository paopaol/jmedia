#include "filter_ascii_chan.h"

namespace JMedia {
	VideoFilterAsciiChan &VideoFilterAsciiChan::movie(const std::string &path)
	{
		char movie[1024] = { 0 };

		//movie=in.avi
		sprintf(movie, "movie=\\'%s\\'", path.c_str());
		m_chan.push_back(movie);

		return *this;
	}
}