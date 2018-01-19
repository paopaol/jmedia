#include "format_writer.h"

namespace JMedia{
    FormatWriter::FormatWriter(const std::string &fileName)
        :m_filename(fileName)
        ,m_output_format_context(nullptr)
    {
        avformat_alloc_output_context2(&m_output_format_context,
                                       NULL, NULL, m_filename.c_str());
    }

    int FormatWriter::add_stream(const Stream *stream)
    {
        AVStream *s = avformat_new_stream(m_output_format_context, NULL);
        if (!s){
            m_error.set(AVERROR(ENOMEM));
            return AVERROR(ENOMEM);
        }

        AVCodec *encoder;
        if (stream->media_type == AVMEDIA_TYPE_VIDEO){
            encoder = avcodec_find_encoder(stream->stream->dec_ctx->codec_id);
            if (!encoder){
                m_error.set(AVERROR_INVALIDDATA);
                return AVERROR_INVALIDDATA;
            }
            AVCodecContext *enc_ctx = avcodec_alloc_context3(encoder);
            if (!enc_ctx){
                m_error.set(AVERROR(ENOMEM));
                return AVERROR(ENOMEM);
            }
            enc_ctx->height = stream->stream->dec_ctx->height;
            enc_ctx->width = stream->stream->dec_ctx->width;
            enc_ctx->sample_aspect_ratio = stream->stream->dec_ctx->sample_aspect_ratio;
            if(encode->pix_fmts){
                enc_ctx->pix_fmt = encoder->pix_fmts[0];
            }else{
                enc_ctx->pix_fmt = stream->stream->dec_ctx->pix_fmt;
            }
            enc_ctx->time_base = av_inv_q(stream->stream->dec_ctx->framerate);
        }else if (stream->media_type = AVMEDIA_TYPE_AUDIO){
            encoder = avcodec_find_encoder(stream->stream->dec_ctx->codec_id);
            if (!encoder){
                m_error.set(AVERROR_INVALIDDATA);
                return AVERROR_INVALIDDATA;
            }
            AVCodecContext *enc_ctx = avcodec_alloc_context3(encoder);
            if (!enc_ctx){
                m_error.set(AVERROR(ENOMEM));
                return AVERROR(ENOMEM);
            }
            enc_ctx->sample_rate = stream->stream->dec_ctx->sample_rate;
            enc_ctx->channel_layout = stream->stream->dec_ctx->channel_layout;
            enc_ctx->sample_fmt = stream->stream->dec_ctx->sample_fmt;
            enc_ctx->time_base = (AVRational){1, enc_ctx->sample_rate};
        }else if (stream->media_type == AVMEDIA_TYPE_UNKNOWN){
            m_error_.set(AVERROR_INVALIDDATA);
            return AVERROR_INVALIDDATA;
        }else {
            int e = avcodec_paramters_copy(s->codecpar, stream->stream->codecpar);
            if (e < 0){
                m_error.set(e);
                return e;
            }
            s->time_base = stream->stream->time_base;
        }
        if (!( m_output_format_context->oformat->flags & AVFMT_NOFILE )){
            int e = avio_open(&m_output_format_context->pb, m_filename.c_str(), AVIO_FLAG_WRITE);
            if (e < 0){
                m_error.set(e);
                return e;
            }
        }
        int e = avformat_write_header(m_output_format_context, NULL);
        if (e < 0){
            m_error.set(e);
            return e;
        }
        return 0;
    }
}
