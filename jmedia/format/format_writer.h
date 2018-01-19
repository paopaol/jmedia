#ifndef JMEDIA_WRITER_H
#define JMEDIA_WRITER_H

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}
#include <string>
#include <list>
#include <Error.h>
#include <base.h>

namespace JMedia{
	class FormatWriter{
		public:
		FormatWriter(const std::string &fileName);
		int add_stream();

		const char *errors(){
            return m_error.what();
        }
    protected:
        error               m_error;
    private:
        std::string 	    m_filename;
        AVFormatContext     *m_output_format_context;
        std::list<Stream>   m_streams;
	};
}

#endif
