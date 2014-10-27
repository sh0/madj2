/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_AO_PULSE
#define H_AO_PULSE

// Int inc
#include "mj_config.h"

// Ext inc
#include <pulse/pulseaudio.h>

#if 0

// Pulse main class
class c_audio_pulse : c_noncopiable
{
    public:
        // Constructor and destructor
        c_audio_pulse(
            std::string& device,
            std::vector<std::string>& channel,
            std::shared_ptr<c_audio_format> aud_fmt
        );
        ~c_audio_pulse();

        // Dispatch
        void dispatch();

        // Frames
        std::shared_ptr<c_audio_frame> frame_open(
            std::string& channel,
            c_audio_source* source
        );
        void frame_close(c_audio_frame* frame, c_audio_source* source);

    private:
        // Pulse client
        pa_threaded_mainloop* m_mainloop;
        pa_mainloop_api* m_api;
        pa_context* m_context;

        // Mutex
        std::mutex m_mutex;

        // Pulse state
        pa_context_state_t m_state;
        pa_operation* m_op;
        pa_stream* m_master;

        // Channels
        class c_channel {
            public:
                // Constructor and destructor
                c_channel(
                    c_audio_pulse* pulse,
                    std::string name,
                    pa_stream* stream
                ) :
                    m_active(false),
                    m_pulse(pulse),
                    m_name(name),
                    m_latency(0),
                    m_ts(0),
                    m_stream(stream)
                { }
                ~c_channel() { if (m_active) final(); }

                // Initialize and finalize
                bool init(pa_stream* master, uint32_t bufsize);
                void final();

                // Name
                std::string& name() { return m_name; }

                // Buffer

                // Latency
                uint32_t latency() { return m_latency; }

            private:
                // Active
                bool m_active;

                // Parent
                c_audio_pulse* m_pulse;

                // Info
                std::string m_name;
                uint32_t m_latency;

                // Timestamp
                int64_t m_ts;

                // Stream
                pa_stream* m_stream;
                static void p_state(pa_stream* str, void* user);
                static void p_write(pa_stream* str, size_t length, void* user);
                static void p_underflow(pa_stream* str, void* user);
        };
        std::list<std::shared_ptr<c_channel>> m_channel;

        // Frames - { source, stream, [ { channel, shared_ptr<frame> }, ... ] }
        struct s_frame_channel {
            c_channel* channel;
            c_audio_frame* frame;
        };

        class c_frame_source : c_noncopiable {
            public:
                // Constructor and destructor
                c_frame_source(
                    c_audio_source* source,
                    std::shared_ptr<c_audio_stream> stream
                ) :
                    m_source(source), m_stream(stream),
                    m_buf_ts(0), m_buf_data(nullptr),
                    m_buf_size(0), m_buf_alloc(0)
                { }
                ~c_frame_source()
                {
                    if (m_buf_data)
                        delete[] m_buf_data;
                }

                // Gets
                c_audio_source* source() { return m_source; }
                std::shared_ptr<c_audio_stream> stream() { return m_stream; }

                // Channels
                bool channel_empty() {
                    return m_channel.empty();
                }
                void channel_add(c_channel* channel, c_audio_frame* frame) {
                    m_channel.push_back({ channel, frame });
                }
                void channel_del(c_audio_frame* frame) {
                    auto it = std::find_if(
                        std::begin(m_channel), std::end(m_channel),
                        [&frame](s_frame_channel& p){ return (p.frame == frame); }
                    );
                    BOOST_ASSERT(it != m_channel.end());
                    m_channel.erase(it);
                }
                s_frame_channel* channel_ref(c_channel* channel) {
                    auto it = std::find_if(
                        std::begin(m_channel), std::end(m_channel),
                        [&](s_frame_channel& p){ return (p.channel == channel); }
                    );
                    BOOST_ASSERT(it != m_channel.end());
                    return &(*it);
                }

                // Buffer
                int64_t buf_ts() { return m_buf_ts; }
                t_sample* buf_data() { return m_buf_data; }
                uint32_t buf_size() { return m_buf_size; }
                void buf_realloc(int64_t ts, uint32_t size) {
                    m_buf_ts = ts;
                    if (size > m_buf_alloc) {
                        m_buf_alloc = size;
                        if (m_buf_data)
                            delete[] m_buf_data;
                        m_buf_data = new t_sample[m_buf_alloc];
                    }
                    m_buf_size = size;
                }

            private:
                // Source, stream and channels
                c_audio_source* m_source;
                std::shared_ptr<c_audio_stream> m_stream;
                std::list<s_frame_channel> m_channel;

                // Buffer
                int64_t m_buf_ts;
                t_sample* m_buf_data;
                uint32_t m_buf_size;
                uint32_t m_buf_alloc;
        };
        std::list<std::unique_ptr<c_frame_source>> m_frame;

        // Pulse callbacks
        static void p_state(pa_context* ctx, void* user);
        static void p_cork(pa_stream* str, int success, void* user);
};

#endif

#endif
