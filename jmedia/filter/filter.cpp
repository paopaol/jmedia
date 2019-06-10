//
// Created by jz on 16-12-26.
//

#include "filter.h"
#include "filter_graph.h"

namespace JMedia{
    Filter::Filter(const std::string &name):
        m_filter(NULL),
        m_filter_ctx(NULL),
		m_name(name)
	{
    }

	Filter::~Filter()
	{
	}

    AVFilterContext* Filter::getAVFilterContext() {
        return m_filter_ctx;
    }

	const std::string Filter::name()const
	{
		return m_name;
	}

	int Filter::init_str()
	{
		int e = avfilter_init_str(m_filter_ctx, NULL);
		m_error.set_error(e);
		return e;
	}
}