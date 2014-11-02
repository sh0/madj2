/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "ff_file_audio.h"

// Constructor and destructor
c_file_audio::c_file_audio(
    int64_t num_frames, int64_t num_subframes, double rate,
    std::shared_ptr<AVCodecContext> context, AVCodec* codec,
    s_codec_audio codec_info, std::shared_ptr<c_file_track> data
) {

}

c_file_audio::~c_file_audio()
{

}

/*
c_file_aud::c_file_aud(
    c_file* file,
    AVCodecContext* aud_ctx,
    AVStream* aud_stream
) :
    m_file(file),
    m_aud_ctx(aud_ctx),
    m_aud_stream(aud_stream)
{
    // Lock
    std::lock_guard<std::mutex> lock(m_mutex);

    // Sample check
    if (m_aud_ctx->sample_rate < 5000)
        throw c_exception("Audio track sample rate too low!", { throw_format("rate", m_aud_ctx->sample_rate) });

    // Channel setting
    m_aud_ctx->request_channels = m_aud_ctx->channels;

    // Buffer sequence
    m_seq = make_unique<c_seq_aud>(10);

    // Source frame
    m_src_frame = std::shared_ptr<AVFrame>(avcodec_alloc_frame(), &av_free);

    // Timing - decoding
    m_time_dec_base = 1.0 / static_cast<double>(m_aud_ctx->sample_rate);
    //TODO: why doesn't av_q2d(m_aud_stream->time_base); work???
    m_time_dec_pts = 0;

    // Timing - playback
    m_time_play_base = 1.0 / static_cast<double>(c_global::audio->format()->freq());
}

c_file_aud::~c_file_aud()
{
    // Lock
    std::lock_guard<std::mutex> lock(m_mutex);

    // Verify that all streams have been destroyed - meaning that all frames
    // have been destroyed and by that invoking the audio driver to delete
    // all streams as well
    BOOST_ASSERT(m_stream.empty());
}

// Decode
double c_file_aud::decode(AVPacket* pck_dec)
{
    // Packet
    AVPacket pck_cur;
    g_memmove(&pck_cur, pck_dec, sizeof(pck_cur));

    // Decode loop
    double ret_time = -1.0;
    int32_t ret_gotframe = 0;
    int32_t ret_size = 0;
    while ((ret_size = avcodec_decode_audio4(m_aud_ctx, m_src_frame.get(), &ret_gotframe, &pck_cur)) >= 0) {

        // Check if frame was successfully decoded
        if (ret_gotframe == 0)
            break;

        // Advance the input frame buffer
        pck_cur.data += ret_size;
        pck_cur.size -= ret_size;

        // Timestamp
        int64_t pts = m_src_frame->best_effort_timestamp;
        if (pts == (int64_t) AV_NOPTS_VALUE)
            pts = m_time_dec_pts;
        m_time_dec_pts = pts + m_src_frame->nb_samples;
        ret_time = m_time_dec_pts * m_time_dec_base;

        // Converter
        c_audio_convert dst_conv{
            m_src_frame.get(),
            m_aud_ctx->sample_fmt,
            m_aud_ctx->channels
        };
        std::shared_ptr<c_audio_raw> dst_buf{dst_conv.conv_mono()};

        // Sequence writing
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_seq->write(pts, dst_buf);
        }

        // Check if data left
        if (pck_cur.size <= 0)
            break;
    }

    // Return time of the last decoded sample
    return ret_time;
}

// Frame
std::shared_ptr<c_audio_frame> c_file_aud::frame_open(std::string& channel)
{
    // Ask audio class to find specified channel, returning new frame or null
    return c_global::audio->frame_open(channel, static_cast<c_audio_source*>(this));
}
*/
