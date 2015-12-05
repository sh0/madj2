/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_FILE_MJV_VIDEO
#define H_MEDIA_FILE_MJV_VIDEO

// Internal
#include "config.hpp"
#include "opengl/image.hpp"
#include "media/file_mjv_track.hpp"

// C++
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

// FFmpeg
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

// File video
class c_file_video : boost::noncopyable
{
    public:
        // Codec info
        struct s_codec_video {
            uint32_t width;
            uint32_t height;
            uint32_t display_width;
            uint32_t display_height;
            uint32_t pixfmt;
        };

        // Constructor and destructor
        c_file_video(
            int64_t num_frames, double rate,
            std::shared_ptr<AVCodecContext> context, AVCodec* codec,
            s_codec_video codec_info, std::shared_ptr<c_file_track> data
        );
        ~c_file_video();

        // Timing
        int64_t time_dts(double tr) {
            int64_t ts = static_cast<int64_t>(tr / m_rate);
            return std::max(static_cast<int64_t>(0), std::min(m_num_frames - 1, ts));
        }
        double time_pts(int64_t ts) {
            return static_cast<double>(ts) * m_rate;
        }

        // Frames
        void frame_load(int64_t ts, int dir) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_decode_ts = ts;
                m_decode_dir = dir;
            }
            m_thread_cond.notify_one();
        }
        std::shared_ptr<c_opengl_image> frame_copy(int64_t ts) {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_sequence.find(ts);
            if (it == m_sequence.end())
                return nullptr;
            return it->second;
        }

    private:
        // Mutex
        std::mutex m_mutex;

        // Frames
        int64_t m_num_frames;
        double m_rate;

        // Codec
        std::shared_ptr<AVCodecContext> m_context;
        AVCodec* m_codec;

        // Video properties
        s_codec_video m_info;
        AVPixelFormat m_pixfmt;

        // Data
        std::shared_ptr<c_file_track> m_data;

        // Source and destination frames
        std::shared_ptr<AVFrame> m_frame;
        SwsContext* m_swscaler;
        std::map<int64_t, std::shared_ptr<c_opengl_image>> m_sequence;

        // Decode thread
        std::thread m_thread;
        std::condition_variable m_thread_cond;
        std::atomic_bool m_thread_run;
        const int64_t m_thread_sleep;
        void decode();

        // Decode timing
        int64_t m_decode_ts;
        int m_decode_dir;
        const int64_t m_decode_buffer;
};

#endif

