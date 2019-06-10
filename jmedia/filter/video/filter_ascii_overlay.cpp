#include "filter_ascii_chan.h"

namespace JMedia {
	VideoFilterAsciiChan &VideoFilterAsciiChan::overlay(int x, int y)
	{
		//overlay=x=10:y=10
		char overlay[1024] = { 0 };

		sprintf(overlay, "overlay=x=%d:y=%d", x, y);
		m_chan.push_back(overlay);
		return *this;
	}

	VideoFilterAsciiChan &VideoFilterAsciiChan::overlay_top_left(int top, int left)
	{
		return overlay(left, top);
	}

	VideoFilterAsciiChan &VideoFilterAsciiChan::overlay_top_right(int top, int right)
	{
		char overlay[1024] = { 0 };

		sprintf(overlay, "overlay=x=main_w-overlay_w-%d:y=%d", right, top);
		m_chan.push_back(overlay);
		return *this;
	}

	VideoFilterAsciiChan &VideoFilterAsciiChan::overlay_bottom_left(int bottom, int left)
	{
		char overlay[1024] = { 0 };

		sprintf(overlay, "overlay=x=%d:y=main_h-overlay_h-%d", left, bottom);
		m_chan.push_back(overlay);
		return *this;
	}

	VideoFilterAsciiChan &VideoFilterAsciiChan::overlay_bottom_right(int bottom, int right)
	{
		char overlay[1024] = { 0 };

		sprintf(overlay, "overlay=x=main_w-overlay_w-%d:y=main_h-overlay_h-%d", right, bottom);
		m_chan.push_back(overlay);
		return *this;

	}
}