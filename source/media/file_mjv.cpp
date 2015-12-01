/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "global.hpp"
#include "media/file_mjv.hpp"
#include "video/video.hpp"

// FFmpeg static helper functions
static std::once_flag p_ffmpeg_once;

static void p_ffmpeg_log(void* ptr, int level, const char* format, va_list vlist)
{
    // Level check
    if (level > AV_LOG_WARNING)
        return;

    // Message
    char msg[1024];
    vsnprintf(msg, 1024, format, vlist);
    std::cout << "FFmpeg: " << msg << std::endl;
}

/*
static std::string p_ffmpeg_averror(int ff_err)
{
    static const int errsize = 128;
    static char errstr[errsize];
    av_make_error_string(errstr, errsize, ff_err);
    return std::string(errstr);
}
*/

static void p_ffmpeg_init()
{
    // Register formats
    av_register_all();

    // Logs
    av_log_set_level(AV_LOG_WARNING);
    av_log_set_callback(p_ffmpeg_log);
}

// Constructor and destructor
c_file::c_file(const boost::filesystem::path& path) :
    // Path
    m_path(path),
    // File
    m_data(nullptr), m_offset(0), m_size(0),
    // Timing
    m_time_duration(0)
{
    // Init FFmpeg
    std::call_once(p_ffmpeg_once, &p_ffmpeg_init);

    // Memory map file
    try {
        // File
        m_file = boost::interprocess::file_mapping(m_path.native().c_str(), boost::interprocess::read_only);
        m_region = boost::interprocess::mapped_region(m_file, boost::interprocess::read_only);

        // Pointers
        m_data = reinterpret_cast<uint8_t*>(m_region.get_address());
        m_size = m_region.get_size();

    } catch (boost::interprocess::interprocess_exception& ex) {

        // Failure
        throw_nested("Failed to open media file:");
    }

    // Tag and version
    uint32_t r_tag = read_u32();
    uint32_t r_version = read_u32();

    // Check
    if (r_tag != 0x4D41444A)
        throw c_exception("Wrong file type!");
    if (r_version != 1)
        throw c_exception("Incorrect file version!");

    // Tracks
    /*
    uint32_t r_num_tracks = read_u32();
    for (uint32_t i = 0; i < r_num_tracks; i++) {

        // Frame info
        uint64_t num_frames = read_u64();
        uint64_t num_subframes = read_u64();
        uint64_t data_offset = read_u64();
        int64_t rate_n = read_u32();
        int64_t rate_d = read_u32();

        // Duration
        m_time_duration = std::max(
            m_time_duration,
            static_cast<double>(rate_n * num_frames * num_subframes) / static_cast<double>(rate_d)
        );

        // Codec info
        uint32_t codec_type = read_u32();
        uint32_t codec_id = read_u32();
        c_file_video::s_codec_video codec_video;
        c_file_audio::s_codec_audio codec_audio;

        // Codec parameters
        if (codec_type == 0) {
            // Video
            codec_video.width = read_u32();
            codec_video.height = read_u32();
            codec_video.display_width = read_u32();
            codec_video.display_height = read_u32();
            codec_video.pixfmt = read_u32();
        } else if (codec_type == 1) {
            // Audio
            codec_audio.sample_rate = read_u32();
            codec_audio.channels = read_u32();
            codec_audio.bits_per_coded_sample = read_u32();
        } else {
            // Unsupported codec type
            throw c_exception("Unknown codec type!", { throw_format("type", codec_type ) });
        }

        // Index
        uint32_t index_size = num_frames * 8;
        auto index = std::unique_ptr<uint8_t[]>(new uint8_t[index_size]);
        read_data(index.get(), index_size);

        // Track data
        auto track_data = std::make_shared<c_file_track>(
            num_frames, m_data + data_offset, m_size - data_offset,
            std::move(index)
        );

        // Codec
        AVCodec* codec = avcodec_find_decoder(static_cast<AVCodecID>(codec_id));
        if (!codec)
            throw c_exception("Could not find decoder!", { throw_format("id", codec_id) });

        // Context
        auto context = std::shared_ptr<AVCodecContext>(
            avcodec_alloc_context3(codec),
            [](AVCodecContext* ctx){ avcodec_close(ctx); av_free(ctx); }
        );
        if (avcodec_open2(context.get(), codec, nullptr) < 0)
            throw c_exception("Could not open codec!", { throw_format("id", codec_id) });

        // Track type
        if (codec_type == 0) {
            // Debug
            std::cout <<
                boost::format("Track %d: video, frames=%d, subframes=%d, rate_n=%d, rate_d=%d, width=%d, height=%d, dwidth=%d, dheight=%d, pixfmt=%d") %
                i % num_frames % num_subframes % rate_n % rate_d %
                codec_video.width % codec_video.height % codec_video.display_width % codec_video.display_height % codec_video.pixfmt
                << std::endl;

            // Video
            auto track = std::make_shared<c_file_video>(
                num_frames, static_cast<double>(rate_n) / static_cast<double>(rate_d),
                context, codec,
                codec_video, track_data
            );
            m_track_video.push_back(track);

        } else if (codec_type == 1) {
            // Debug
            std::cout <<
                boost::format("Track %d: audio, frames=%d, subframes=%d, rate_n=%d, rate_d=%d, sample_rate=%d, channels=%d, bits_per_sample=%d") %
                i % num_frames % num_subframes % rate_n % rate_d %
                codec_audio.sample_rate % codec_audio.channels % codec_audio.bits_per_coded_sample
                << std::endl;

            // Audio
            auto track = std::make_shared<c_file_audio>(
                num_frames, num_subframes, static_cast<double>(rate_n) / static_cast<double>(rate_d),
                context, codec,
                codec_audio, track_data
            );
            m_track_audio.push_back(track);

        } else {
            // Unknown track type
            continue;
        }
    }
    */
}

c_file::~c_file()
{
    // Tracks
    m_track_video.clear();
}

// File reading
uint8_t c_file::read_u8()
{
    read_check(1);
    return m_data[m_offset++];
}

uint16_t c_file::read_u16()
{
    read_check(2);
    uint16_t tmp = (m_data[m_offset] << 8) | m_data[m_offset + 1];
    m_offset += 2;
    return tmp;
}

uint32_t c_file::read_u32()
{
    read_check(4);
    uint32_t tmp = 0;
    for (int i = 0; i < 4; i++)
        tmp = (tmp << 8) | m_data[m_offset++];
    return tmp;
}

uint64_t c_file::read_u64()
{
    read_check(8);
    uint64_t tmp = 0;
    for (int i = 0; i < 8; i++)
        tmp = (tmp << 8) | m_data[m_offset++];
    return tmp;
}

void c_file::read_check(uint64_t size)
{
    if (m_size - m_offset < size)
        throw c_exception("Invalid data!");
}

void c_file::read_data(uint8_t* data, uint32_t size)
{
    read_check(size);
    memcpy(data, m_data + m_offset, size);
    m_offset += size;
}
