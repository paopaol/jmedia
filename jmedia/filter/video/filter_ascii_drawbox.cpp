#include "filter_ascii_chan.h"

namespace JMedia {
    VideoFilterAsciiChan &VideoFilterAsciiChan::drawbox(int x, int y,
        int w, int h,
        int thickness, double opacity, 
        unsigned int color,
        bool invert)
    {
        //drawbox=x=10:y=10:w=100:h=100:color=0xFFFFFF@0.5:t=3
        char drawbox[256] = { 0 };
        sprintf(drawbox, "drawbox=x=%d"
            ":y=%d"
            ":w=%d"
            ":h=%d"
            //":invert=%d"
            ":color=0x%X"
            "@%.2f"
            ":t=%d", 
            x, y, w, h, /*invert,*/ color, opacity, thickness);
        m_chan.push_back(drawbox);
        return *this;
    }
}