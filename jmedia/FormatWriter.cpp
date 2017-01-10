//
// Created by jz on 17-1-10.
//

#include "FormatWriter.h"

namespace JMedia{
    FormatWriter::FormatWriter(const std::string &filename)
            :m_output_format_context(nullptr)
    {
        m_output_file = filename;
        avformat_alloc_output_context2(&m_output_format_context, NULL, NULL, filename.c_str());
        if (m_output_format_context == nullptr){
            m_error.set_error(AVERROR_UNKNOWN);
        }
    }
}