#ifndef FILTER_ASCII_GRAPH_H
#define FILTER_ASCII_GRAPH_H

#include <vector>
#include <string>

namespace JMedia {
	class VideoFilterAsciiChan;
	class VideoFilterAsciiGraph {
	public:
		VideoFilterAsciiGraph &push_chan(const VideoFilterAsciiChan chan);
		std::string string();
		VideoFilterAsciiGraph &reset();

		std::vector<VideoFilterAsciiChan> m_chanList;
	};

}

#endif