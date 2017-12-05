//
// Created by jz on 16-12-25.
//

#ifndef DECODE_AUDIO_FILTERCONFIG_H
#define DECODE_AUDIO_FILTERCONFIG_H

#include <string>


extern "C" {
#include <libavfilter/avfilter.h>
#include <libavutil/samplefmt.h>
#include <libavutil/rational.h>
}

#include <Error.h>



namespace JMedia {
	class FilterGraph;
    class Filter {
    public:
        Filter(const std::string &name);
		~Filter();


        AVFilterContext *getAVFilterContext();
		const std::string name()const;
		int init_str();
        const char *errors(){
            return m_error.what();
        }

    protected:
        AVFilterContext                 *m_filter_ctx;
        AVFilter                        *m_filter;
		std::string						*m_args;
		error							m_error;
    private:
		mutable std::string				m_name;
    };
}

#endif //DECODE_AUDIO_FILTERCONFIG_H
