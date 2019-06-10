#include "filter_ascii_chan.h"

namespace JMedia {
	VideoFilterAsciiChan &VideoFilterAsciiChan::in(const std::string &name)
	{
		m_in.push_back(name);
		return *this;
	}

	VideoFilterAsciiChan &VideoFilterAsciiChan::output(const std::string &name)
	{
		m_out.push_back(name);
		return *this;
	}




	std::string VideoFilterAsciiChan::string()
	{
		std::string chan;

		chan = prepend_in();

		std::string last = m_chan.size() > 0 ? m_chan[0] : "";
		for (unsigned int i = 0; i < m_chan.size() - 1 &&
			m_chan.size() > 0; i++) {
			chan.append(m_chan[i]).append(",");
			last = m_chan[i + 1];
		}
		chan.append(last);

		chan.append(append_out());
		return chan;
	}


	VideoFilterAsciiChan &VideoFilterAsciiChan::reset()
	{
		m_chan.resize(0);
		m_in.resize(0);
		m_out.resize(0);

		return *this;
	}

	std::string VideoFilterAsciiChan::prepend_in()
	{
		std::string	input;

		for (auto name = m_in.begin(); name != m_in.end(); name++) {
			input += "[" + *name + "] ";
		}
		return input;
	}

	std::string VideoFilterAsciiChan::append_out()
	{
		std::string	output;

		for (auto name = m_out.begin(); name != m_out.end(); name++) {
			output += " [" + *name + "] ";
		}
		return output;
	}
}