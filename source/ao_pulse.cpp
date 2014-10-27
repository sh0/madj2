/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Int inc
#include "mj_config.h"
#include "ao_pulse.h"

#if 0

// Initialize and finalize
bool c_audio_pulse::init(
    std::string& device,
    std::vector<std::string>& channel,
    std::shared_ptr<c_audio_format> aud_fmt
) {
    // Check
    BOOST_ASSERT(!m_active);

    // Mainloop
    m_mainloop = pa_threaded_mainloop_new();
    if (!m_mainloop) {
        msg_critical("Pulseaudio: Failed to open mainloop!");
        return false;
    }
    m_api = pa_threaded_mainloop_get_api(m_mainloop);

    // Context create
    m_context = pa_context_new(m_api, "madj");
    if (!m_context) {
        msg_critical("Pulseaudio: Failed to create context!");
        return false;
    }

    // Context connect
    {
        int ret = pa_context_connect(
            m_context,
            device.empty() ? nullptr : device.c_str(),
            PA_CONTEXT_NOAUTOSPAWN,
            nullptr
        );
        if (ret != PA_OK) {
            msg_critical(
                boost::format("Pulseaudio: Failed to connect context! error=%s") %
                pa_strerror(ret)
            );
            return false;
        }
    }

    // State callback
    pa_context_set_state_callback(m_context, c_audio_pulse::p_state, this);

    // Start mainloop
    if (pa_threaded_mainloop_start(m_mainloop) < 0) {
        msg_critical("Pulseaudio: Failed to start mainloop!");
        return false;
    }

    // Wait for ready status
    {
        pa_threaded_mainloop_lock(m_mainloop);
        bool ret = false;
        bool loop = true;
        while (loop) {
            switch (m_state) {
                // Failure
                case PA_CONTEXT_FAILED:
                case PA_CONTEXT_TERMINATED:
                    loop = false;
                    ret = false;
                    break;

                // Success
                case PA_CONTEXT_READY:
                    loop = false;
                    ret = true;
                    break;

                // Unimportant
                case PA_CONTEXT_UNCONNECTED:
                case PA_CONTEXT_CONNECTING:
                case PA_CONTEXT_AUTHORIZING:
                case PA_CONTEXT_SETTING_NAME:
                default:
                    break;
            }
            if (loop)
                pa_threaded_mainloop_wait(m_mainloop);
        }
        pa_threaded_mainloop_unlock(m_mainloop);
        if (!ret) {
            msg_critical("Pulseaudio: Failed to obtain ready state!");
            return false;
        }
    }

    // Format initialization
    if (!aud_fmt->is_active())
        aud_fmt->init(44100);

    // Channels
    {
        // Lock
        pa_threaded_mainloop_lock(m_mainloop);

        // Generate mapping for all channels
        pa_channel_map map_all;
        pa_channel_map_init_extend(
            &map_all,
            channel.size(),
            PA_CHANNEL_MAP_DEFAULT
        );

        // Master stream
        m_master = nullptr;

        // Loop channels
        uint32_t p = 0;
        for (auto &it : channel) {
            // Check name
            if (it.empty()) {
                msg_critical(
                    boost::format("Pulseaudio: Empty channel name not allowed! index=%d") %
                    p
                );
                continue;
            }

            // Sample spec
            pa_sample_spec ss;
            ss.format = PA_SAMPLE_FLOAT32LE;
            ss.rate = aud_fmt->freq();
            ss.channels = 1;

            // Map
            pa_channel_map map;
            map.channels = 1;
            map.map[0] = map_all.map[p];

            // Stream creation
            pa_stream* str = pa_stream_new(m_context, it.c_str(), &ss, &map);
            if (!str) {
                msg_critical(
                    boost::format("Pulseaudio: failed to create stream! name=%s") %
                    it
                );
                continue;
            }

            // Channel
            msg_warning(boost::format("Pulseaudio: Channel! name=%s") % it);
            auto ch = std::make_shared<c_channel>(this, it, str);
            bool ret = ch->init(
                m_master,
                pa_usec_to_bytes(aud_fmt->latency_usec(), &ss)
            );
            if (ret) {
                // Add to channel list
                m_channel.push_back(ch);

                // Set master stream
                if (!m_master)
                    m_master = str;
            } else {
                // Unref stream
                pa_stream_unref(str);
            }

            // Index
            p++;
        }

        // Uncork master
        if (m_master) {
            m_op = pa_stream_cork(m_master, 0, c_audio_pulse::p_cork, this);
            if (m_op) {
                while (pa_operation_get_state(m_op) == PA_OPERATION_RUNNING)
                    pa_threaded_mainloop_wait(m_mainloop);
                pa_operation_unref(m_op);
            } else {
                int err = pa_context_errno(m_context);
                msg_critical(
                    boost::format("Pulseaudio: pa_stream_cork failed! error=%s") %
                    pa_strerror(err)
                );
            }
        }

        // Unlock
        pa_threaded_mainloop_unlock(m_mainloop);
    }

    // Success
    m_active = true;
    return true;
}

void c_audio_pulse::final()
{
    // Check
    BOOST_ASSERT(m_active);

    // Lock
    pa_threaded_mainloop_lock(m_mainloop);

    // Channels
    m_channel.clear();

    // Context
    pa_context_disconnect(m_context);
    pa_context_unref(m_context);

    // Unlock
    pa_threaded_mainloop_unlock(m_mainloop);

    // Mainloop
    pa_threaded_mainloop_stop(m_mainloop);
    pa_threaded_mainloop_free(m_mainloop);

    // Deactivate
    m_active = false;
}

// Dispatch
bool c_audio_pulse::dispatch()
{
    // Check
    BOOST_ASSERT(m_active);

    // State
    pa_threaded_mainloop_lock(m_mainloop);
    bool ok = (m_state == PA_CONTEXT_READY);
    pa_threaded_mainloop_unlock(m_mainloop);

    // Return
    return ok;
}

// Frames
std::shared_ptr<c_audio_frame> c_audio_pulse::frame_open(
    std::string& channel,
    c_audio_source* source
) {
    // Check
    BOOST_ASSERT(m_active);

    // Lock
    pa_threaded_mainloop_lock(m_mainloop);

    // Try to find channel
    auto it_c = std::find_if(
        std::begin(m_channel),
        std::end(m_channel),
        [&channel](std::shared_ptr<c_channel>& p){
            return (channel == p->name());
        }
    );
    if (it_c == m_channel.end())
        return nullptr;

    // Try to find source
    auto it_s = std::find_if(
        std::begin(m_frame),
        std::end(m_frame),
        [&source](std::unique_ptr<c_frame_source>& p){
            return (source == p->source());
        }
    );
    if (it_s == m_frame.end())
        it_s = m_frame.insert(
            m_frame.end(),
            make_unique<c_frame_source>(source, source->stream_open())
        );

    // New frame
    auto frame = std::make_shared<c_audio_frame>(std::bind(
        &c_audio_pulse::frame_close, this,
        std::placeholders::_1, source
    ));

    // Add to list
    (*it_s)->channel_add(it_c->get(), frame.get());

    // Unlock
    pa_threaded_mainloop_unlock(m_mainloop);

    // Return
    return frame;
}

void c_audio_pulse::frame_close(c_audio_frame* frame, c_audio_source* source)
{
    // Check
    BOOST_ASSERT(m_active);

    // Lock
    pa_threaded_mainloop_lock(m_mainloop);

    // Must find the source
    auto it_s = std::find_if(
        std::begin(m_frame),
        std::end(m_frame),
        [&source](std::unique_ptr<c_frame_source>& p){
            return (source == p->source());
        }
    );
    BOOST_ASSERT(it_s != m_frame.end());

    // Must find the frame to remove it
    (*it_s)->channel_del(frame);

    // If list empty then delete the stream and remove source from list
    if ((*it_s)->channel_empty()) {
        source->stream_close((*it_s)->stream());
        m_frame.erase(it_s);
    }

    // Unlock
    pa_threaded_mainloop_unlock(m_mainloop);
}

// Channels
bool c_audio_pulse::c_channel::init(pa_stream* master, uint32_t bufsize)
{
    // Check
    BOOST_ASSERT(!m_active);

    // Callbacks
    pa_stream_set_state_callback(
        m_stream,
        c_audio_pulse::c_channel::p_state,
        this
    );
    pa_stream_set_write_callback(
        m_stream,
        c_audio_pulse::c_channel::p_write,
        this
    );
    pa_stream_set_underflow_callback(
        m_stream,
        c_audio_pulse::c_channel::p_underflow,
        this
    );

    // Buffer attributes
    pa_buffer_attr bufattr;
    bufattr.fragsize = (uint32_t) -1;
    bufattr.maxlength = (uint32_t) -1;
    bufattr.minreq = (uint32_t) -1;
    bufattr.prebuf = 0; // needed for server side underrun desync protection
    bufattr.tlength = bufsize;

    // Connect
    int ret = pa_stream_connect_playback(
        m_stream, nullptr, &bufattr,
        static_cast<pa_stream_flags_t>(
            PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_ADJUST_LATENCY |
            PA_STREAM_AUTO_TIMING_UPDATE | PA_STREAM_START_CORKED
        ),
        nullptr, master
    );
    if (ret < 0) {
        msg_critical(
            boost::format("Pulseaudio: Unable to connect stream! name=%s") %
            m_name
        );
        return false;
    }

    // Stream state
    pa_stream_state_t state;
    while ((state = pa_stream_get_state(m_stream)) == PA_STREAM_CREATING)
        pa_threaded_mainloop_wait(m_pulse->m_mainloop);
    switch (state) {
        case PA_STREAM_UNCONNECTED:
            msg_critical("Pulseaudio: Stream unconnected!");
            return false;
        case PA_STREAM_FAILED:
            msg_critical("Pulseaudio: Stream failed!");
            return false;
        case PA_STREAM_TERMINATED:
            msg_critical("Pulseaudio: Stream terminated!");
            return false;
        default:
            break;
    }

    // Success
    m_active = true;
    return true;
}

void c_audio_pulse::c_channel::final()
{
    // Check
    BOOST_ASSERT(m_active);

    // Disconnect
    pa_stream_disconnect(m_stream);

    // Unref
    pa_stream_unref(m_stream);

    // Deactivate
    m_active = false;
}

void c_audio_pulse::c_channel::p_state(pa_stream* str, void* user)
{
    // Instance
    c_audio_pulse::c_channel* channel =
        reinterpret_cast<c_audio_pulse::c_channel*>(user);

    // Signal
    pa_threaded_mainloop_signal(channel->m_pulse->m_mainloop, 0);
}

void c_audio_pulse::c_channel::p_write(pa_stream* str, size_t length, void* user)
{
    // Instance
    c_audio_pulse::c_channel* channel =
        reinterpret_cast<c_audio_pulse::c_channel*>(user);

    // Latency
    pa_usec_t usec;
    int neg;
    if (pa_stream_get_latency(str, &usec, &neg) == 0)
        channel->m_latency = usec;

    // Begin
    t_sample* wr_data = nullptr;
    size_t wr_size = length;
    if (pa_stream_begin_write(
            str, reinterpret_cast<void**>(&wr_data), &wr_size
        ) < 0) {
        msg_critical(
            boost::format("Pulseaudio: stream_begin_write failed! name=%s") %
            channel->m_name
        );
        return;
    }

    // Check
    if (!wr_data || !wr_size)
        return;

    // Set samples to 0.0f
    memset(wr_data, 0, wr_size);

    // Loop channels
    for (auto &it_s : channel->m_pulse->m_frame) {
        auto it_c = it_s->channel_ref(channel);
        if (it_c) {

            // Buffer loading
            if ((it_s->buf_ts() != channel->m_ts) || (!it_s->buf_data())) {
                it_s->buf_realloc(channel->m_ts, wr_size / sizeof(t_sample));
                auto cb_write = [&it_s](t_sample* data, uint32_t size){
                    memcpy(it_s->buf_data(), data, size * sizeof(t_sample));
                };
                it_s->stream()->process(
                    cb_write,
                    wr_size / sizeof(t_sample),
                    channel->m_latency
                );
            }

            // Write
            it_c->frame->process(
                it_s->buf_data(),
                wr_data,
                MIN(wr_size / sizeof(t_sample), it_s->buf_size())
            );

        }
    }

    // Timestamp
    channel->m_ts += wr_size / sizeof(t_sample);

    // Commit
    if (pa_stream_write(str, wr_data, wr_size, nullptr, 0, PA_SEEK_RELATIVE) < 0) {
        msg_critical(
            boost::format("Pulseaudio: pa_stream_write failed! name=%s") %
            channel->m_name
        );
        return;
    }
}

void c_audio_pulse::c_channel::p_underflow(pa_stream* str, void* user)
{
    msg_warning("Pulseaudio: Stream underflow!");
}

// Pulse callbacks
void c_audio_pulse::p_state(pa_context* ctx, void* user)
{
    // Instance
    c_audio_pulse* pulse = reinterpret_cast<c_audio_pulse*>(user);

    // State
    pulse->m_state = pa_context_get_state(ctx);

    // Signal
    pa_threaded_mainloop_signal(pulse->m_mainloop, 0);
}

void c_audio_pulse::p_cork(pa_stream* str, int success, void* user)
{
    // Instance
    c_audio_pulse* pulse = reinterpret_cast<c_audio_pulse*>(user);

    // Signal
    pa_threaded_mainloop_signal(pulse->m_mainloop, 0);
}

#endif
