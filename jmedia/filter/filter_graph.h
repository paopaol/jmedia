//
// Created by jz on 16-12-25.
//

#ifndef DECODE_AUDIO_FILTERGRAPH_H
#define DECODE_AUDIO_FILTERGRAPH_H

#include <string>


extern "C" {
#include <libavfilter/avfilter.h>
#include <libavutil/samplefmt.h>
#include <libavutil/rational.h>
}

#include <Error.h>
#include <list>

namespace JMedia{
	class Filter;
    class FilterGraph {
    public:
        FilterGraph();
        ~FilterGraph();
        AVFilterGraph *getAVFilterGraph();
        int config(const std::string &filters_descr);
        int set_src_sink(Filter &src, Filter &sink);
        int src_add_frame(AVFrame *frame);
		int sink_get_frame(std::list<AVFrame *> &filteredFrameList);
        const char *errors(){
            return m_error.what();
        }

    protected:
		error					m_error;

    private:
        AVFilterGraph           *m_filter_graph;
        AVFilterContext         *m_src;
        AVFilterContext         *m_sink;
		AVFrame					*m_frame;
    };

}


#endif //DECODE_AUDIO_FILTERGRAPH_H
