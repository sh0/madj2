/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_FF_FILE_AUDIO
#define H_FF_FILE_AUDIO

// Internal
#include "mj_config.h"
#include "ff_file_track.h"

// C++
#include <thread>
#include <mutex>

// FFmpeg
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

// File audio
class c_file_audio : c_noncopiable
{
    public:
        // Codec info
        struct s_codec_audio {
            uint32_t sample_rate;
            uint32_t channels;
            uint32_t bits_per_coded_sample;
        };

        // Constructor and destructor
        c_file_audio(
            int64_t num_frames, int64_t num_subframes, double rate,
            std::shared_ptr<AVCodecContext> context, AVCodec* codec,
            s_codec_audio codec_info, std::shared_ptr<c_file_track> data
        );
        ~c_file_audio();

    private:
        // Mutex
        std::mutex m_mutex;

        // Frames
        int64_t m_num_frames;
        int64_t m_num_subframes;
        double m_rate;

        // Codec
        std::shared_ptr<AVCodecContext> m_context;
        AVCodec* m_codec;
};

#endif
