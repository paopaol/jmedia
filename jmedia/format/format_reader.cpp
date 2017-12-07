//
// Created by jz on 16-12-24.
//

#include <list>
#include <string>
#include <assert.h>

#include "format_reader.h"
#include <stdio.h>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/dict.h>
}

namespace JMedia {

    FormatReader::FormatReader(const std::string &filename):
		m_filename(filename),
		m_input_format_context(NULL)
    {
    }

    FormatReader::~FormatReader() 
	{
    }

	int FormatReader::close()
	{
        for (auto stream = m_streams.begin(); stream != m_streams.end(); stream++){
            AVCodecContext  *codec_context = stream->codec_context;
            if (codec_context){
                avcodec_free_context(&codec_context);
            }
        }
        if (m_input_format_context) {
            avformat_close_input(&m_input_format_context);
        }
		return 0;
	}

	map<string, string> FormatReader::metadata()
	{
		assert(m_input_format_context != nullptr);

		AVDictionaryEntry *tag = NULL;
		map<string, string> metadata;

		while ((tag = av_dict_get(m_input_format_context->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
			metadata[tag->key] = tag->value;
		}
		return metadata;
	}



    int FormatReader::open() 
	{
        int error_code;
        AVCodec *input_codec = NULL;

        error_code = avformat_open_input(&m_input_format_context, m_filename.c_str(), NULL, NULL);
        if (error_code < 0) {
            m_error.set_error(error_code); 
            return error_code;
        }
        error_code = avformat_find_stream_info(m_input_format_context, NULL);
        if (error_code < 0) {
            m_error.set_error(error_code); 
            return error_code;
        }
        for (unsigned int i = 0; i < m_input_format_context->nb_streams; i++) {
            AVMediaType media_type = m_input_format_context->streams[i]->codecpar->codec_type;
            AVCodecID codec_id = m_input_format_context->streams[i]->codecpar->codec_id;
            AVCodecParameters *codecpar = m_input_format_context->streams[i]->codecpar;
            int stream_index = i;

            input_codec = avcodec_find_decoder(codec_id);
            if (!input_codec) {
                continue;
            }

            AVCodecContext *codec_context = avcodec_alloc_context3(input_codec);
            if (!codec_context) {
                m_error.set_error(AVERROR(ENOMEM)); 
                return AVERROR(ENOMEM);
            }
            error_code = avcodec_parameters_to_context(codec_context, codecpar);
            if (error_code < 0) {
                m_error.set_error(error_code); 
                return error_code;
            }
            error_code = avcodec_open2(codec_context, input_codec, NULL);
            if (error_code < 0) {
                m_error.set_error(error_code); 
                return error_code;
            }
            Decoder decoder = Decoder(codec_context);
            Stream stream = {media_type,codec_context, decoder, stream_index};
            m_streams.push_back(stream);
        }
        return 0;
    }


    int FormatReader::read_packet(Packet &pkt)
	{
        int error = 0;

        if (m_input_format_context == nullptr){
			m_error.set_error(AVERROR_INVALIDDATA);
			return AVERROR_INVALIDDATA;
        }

        error = av_read_frame(m_input_format_context, &pkt.m_pkt);
        if (error < 0) {
			m_error.set_error(error);
			return error;
        }
        return error;
    }


    AVMediaType FormatReader::media_type(Packet &pkt) 
	{
        int stream_index = pkt.m_pkt.stream_index;

        for (auto it = m_streams.begin(); it != m_streams.end(); it++){
            if (it->stream_index == stream_index){
                return it->media_type;
            }
        }

        return AVMEDIA_TYPE_UNKNOWN;
    }

	int FormatReader::find_decoder(AVMediaType media_type, Decoder &decoder)
	{
        for (auto it = m_streams.begin(); it != m_streams.end(); it++){
            if (it->media_type == media_type){
                decoder = it->decoder;
				return 0;
            }
        }
		m_error.set_error(AVERROR_DECODER_NOT_FOUND);
		return AVERROR_DECODER_NOT_FOUND;
    }


	int FormatReader::getCodecContext(AVMediaType media_type, AVCodecContext *&codecContext)
	{
        for (auto it = m_streams.begin(); it != m_streams.end(); it++){
            if (it->media_type == media_type){
                codecContext = it->codec_context;
				return 0;
            }
        }
		m_error.set_error(AVERROR_DECODER_NOT_FOUND);
		return AVERROR_DECODER_NOT_FOUND;
    }

}


