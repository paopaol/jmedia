#ifndef FILTERS_VIDEO_H
#define FILTERS_VIDEO_H

#include "filter_ascii_chan.h"
#include "filter_ascii_graph.h"
#include <vector>
#include <string>

namespace JMedia {
	VideoFilterAsciiGraph &VideoFilterAsciiGraph::push_chan(const VideoFilterAsciiChan chan)
	{
		m_chanList.push_back(chan);
		return *this;
	}

	std::string VideoFilterAsciiGraph::string()
	{
		std::string graph;

		VideoFilterAsciiChan last = m_chanList.size() > 0 ? 
			m_chanList[0] : VideoFilterAsciiChan();
		
		for (unsigned int i = 0; i < m_chanList.size() - 1 &&
			m_chanList.size() > 0; i++) {
			graph.append(m_chanList[i].string()).append(";\n");
			last = m_chanList[i + 1];
		}
		graph.append(last.string());
		return graph;
	}

	VideoFilterAsciiGraph &VideoFilterAsciiGraph::reset()
	{
		m_chanList.clear();
		return *this;
	}
}

#endif