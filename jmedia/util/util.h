#ifndef JMEDIA_UTIL_H
#define JMEDIA_UTIL_H

extern "C" {
#include <libavutil/avutil.h>
}

namespace JMedia {
namespace Util {
    bool av_time_before(int64_t a_pts, AVRational a_timebase, int64_t b_pts,
                        b_timebase);
    bool av_time_after(int64_t a_pts, AVRational a_timebase, int64_t b_pts,
                       b_timebase);
    bool av_time_equal(int64_t a_pts, AVRational a_timebase, int64_t b_pts,
                       b_timebase);
}  // namespace Util

}  // namespace JMedia

#endif
