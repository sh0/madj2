/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "ff_file_video.h"

// Constants
static const int64_t p_buffer_video_seconds = 1;
static const int64_t p_video_decoder_sleep_ms = 20;

// Constructor and destructor
c_file_video::c_file_video(
    int64_t num_frames, double rate,
    std::shared_ptr<AVCodecContext> context, AVCodec* codec,
    s_codec_video codec_info, std::shared_ptr<c_file_track> data
) :
    // Frames
    m_num_frames(num_frames), m_rate(rate),
    // Codec
    m_context(context), m_codec(codec),
    // Video properties
    m_info(codec_info), m_data(data),
    // Source and destination frames
    m_swscaler(nullptr),
    // Decode thread
    m_thread_run(true), m_thread_sleep(p_video_decoder_sleep_ms),
    // Decode timing
    m_decode_ts(0), m_decode_dir(1),
    m_decode_buffer(p_buffer_video_seconds / rate)
{
    // Lock
    std::lock_guard<std::mutex> lock(m_mutex);

    // Frame size
    if (m_info.width == 0 || m_info.height == 0) {
        throw c_exception(
            "Incorrect video frame size!", {
                throw_format("width", m_info.width),
                throw_format("height", m_info.height)
            }
        );
    }
    m_context->width = m_info.width;
    m_context->height = m_info.height;

    // Display size
    if (m_info.display_width <= 0 || m_info.display_height <= 0) {
        m_info.display_width = m_info.width;
        m_info.display_height = m_info.height;
    }
    m_context->sample_aspect_ratio.num = m_info.display_width;
    m_context->sample_aspect_ratio.den = m_info.display_height;

    // Pixel format
    m_pixfmt = AV_PIX_FMT_NONE;
    if (m_info.pixfmt != 0) {
        for (int i = AV_PIX_FMT_NONE; i < AV_PIX_FMT_NB; i++) {
            if (avcodec_pix_fmt_to_codec_tag(static_cast<AVPixelFormat>(i)) == m_info.pixfmt) {
                m_context->pix_fmt = static_cast<AVPixelFormat>(i);
                break;
            }
        }
    }
    if (m_pixfmt != AV_PIX_FMT_NONE)
        m_context->pix_fmt = m_pixfmt;

    // Decoding frame
    m_frame = std::shared_ptr<AVFrame>(avcodec_alloc_frame(), &av_free);

    // Decode thread
    m_thread = std::thread(&c_file_video::decode, this);
}

c_file_video::~c_file_video()
{
    // Decode thread
    m_thread_run = false;
    m_thread_cond.notify_all();
    if (m_thread.joinable())
        m_thread.join();

    // Lock (to make sure decoding function has finished)
    std::lock_guard<std::mutex> lock(m_mutex);

    // Free scaler
    if (m_swscaler)
        sws_freeContext(m_swscaler);
}

// Decode
void c_file_video::decode()
{
    // Decoding loop
    while (true) {
        // Signal waiting
        int64_t ts = -1;
        while (true) {
            // Check exit
            if (!m_thread_run.load())
                return;

            // Lock
            std::unique_lock<std::mutex> lock(m_mutex);

            // Timestamp
            ts = std::max(0L, std::min(m_num_frames - 1, m_decode_ts));
            int64_t ts_min = std::max(0L, std::min(m_num_frames - 1, ts - m_decode_buffer));
            int64_t ts_max = std::max(0L, std::min(m_num_frames - 1, ts + m_decode_buffer));

            // Clear sequence frames that are out of buffering area
            m_sequence.erase(m_sequence.begin(), m_sequence.upper_bound(ts_min - 1));
            m_sequence.erase(m_sequence.lower_bound(ts_max + 1), m_sequence.end());

            // Select timestamp to decode
            if (m_decode_dir > 0) {
                // Buffer forward
                while (ts <= ts_max && m_sequence.count(ts))
                    ts++;
                if (ts <= ts_max)
                    break;

            } else if (m_decode_dir < 0) {
                // Buffer backwards
                while (ts >= ts_min && m_sequence.count(ts))
                    ts--;
                if (ts >= ts_min)
                    break;

            } else {
                // Buffering disabled
                if (!m_sequence.count(ts))
                    break;
            }

            // Thread
            m_thread_cond.wait_for(lock, std::chrono::milliseconds(m_thread_sleep));
        }

        // Get data
        auto chunk = m_data->read(ts);
        if (!chunk)
            continue;

        // Packet
        AVPacket packet;
        av_init_packet(&packet);
        packet.size = chunk->size();
        packet.data = chunk->data();
        packet.pts = ts;
        packet.dts = ts;

        // Debug
        /*
        msg_message(boost::format("decode! ts=%1%, size=%2%, data=%3%") % ts % packet.size % reinterpret_cast<uint64_t>(packet.data));
        FILE* fs = fopen("frame.jpg", "wb");
        fwrite(packet.data, packet.size, 1, fs);
        fclose(fs);
        */

        // Decode
        int got_frame = 0;
        int ret_frame = avcodec_decode_video2(m_context.get(), m_frame.get(), &got_frame, &packet);
        if (ret_frame <= 0 || got_frame == 0)
            continue;

        // Target image
        auto image = std::make_shared<c_image>(
            c_image::e_type::rgb24,
            m_info.width, m_info.height
        );

        // Target format detection
        m_pixfmt = m_context->pix_fmt;

        // Set up scaler
        AVPixelFormat dstfmt = PIX_FMT_RGB24;
        m_swscaler = sws_getCachedContext(
            m_swscaler,
            m_info.width, m_info.height, m_pixfmt,
            m_info.width, m_info.height, dstfmt,
            SWS_POINT, nullptr, nullptr, nullptr
        );
        if (!m_swscaler)
            continue;

        // Run conversion
        AVPicture dstpic;
        memset(&dstpic, 0, sizeof(dstpic));
        avpicture_fill(&dstpic, image->data(), dstfmt, m_info.width, m_info.height);
        sws_scale(
            m_swscaler,
            m_frame->data, m_frame->linesize, 0, m_info.height,
            dstpic.data, dstpic.linesize
        );

        // Save in sequence
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_sequence.insert(std::make_pair(ts, image));
        }

        // Release
        #ifdef av_frame_unref
        if (m_context->refcounted_frames)
            av_frame_unref(m_frame.get());
        #endif
    }
}
