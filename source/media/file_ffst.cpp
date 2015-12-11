/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "media/file_ffst.hpp"

// Boost
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>

// Constructor and destructor
c_media_file_ffst::c_media_file_ffst(boost::filesystem::path path)
{
    // Debug
    //std::cout << boost::format("FFST: Opening file! path = %1%") % path << std::endl;
    //av_log_set_level(AV_LOG_TRACE);
    //av_log_set_callback(av_log_default_callback);

    // Errors
    auto averror = [](int ret) {
        std::string errstr = "Unknown error!";
        std::array<char, 1024> errbuf;
        if (!av_strerror(ret, errbuf.data(), errbuf.size()))
            errstr = errbuf.data();
        return errstr;
    };

    // Format
    AVFormatContext* format_context_ptr = nullptr;
    int format_result = avformat_open_input(&format_context_ptr, path.c_str(), nullptr, nullptr);
    if (format_result)
        throw c_exception("FFST: Failed to open file!", { throw_format("path", path), throw_format("error", averror(format_result)) });
    auto format_context = std::shared_ptr<AVFormatContext>(
        format_context_ptr,
        [](AVFormatContext* ctx){ avformat_close_input(&ctx); }
    );

    // Stream
    AVCodec* codec_decoder = nullptr;
    int stream = av_find_best_stream(format_context.get(), AVMEDIA_TYPE_SUBTITLE, -1, -1, &codec_decoder, 0);
    if (stream < 0 || !codec_decoder)
        throw c_exception("FFST: Failed to find subtitle stream!", { throw_format("path", path) });

    // Codec
    auto codec_context = format_context->streams[stream]->codec;
    int codec_result = avcodec_open2(codec_context, codec_decoder, nullptr);
    if (codec_result < 0)
        throw c_exception("FFST: Failed to open subtitle decoder!", { throw_format("path", path), throw_format("error", averror(codec_result)) });

    // Timing
    /*
    std::cout << boost::format("FFST: stream_timebase = %f, codec_timebase = %f") %
        av_q2d(format_context->streams[stream]->time_base) %
        av_q2d(codec_context->time_base) << std::endl;
    */
    double timebase = av_q2d(format_context->streams[stream]->time_base);

    // Format specific parsing
    auto parse_ass = [&](std::string line) {
        // Validate
        if (!boost::starts_with(line, "Dialogue:"))
            return std::vector<std::string>();

        // Extract content
        auto find = boost::find_nth(line, ",", 8);
        if (find.end() == line.end())
            return std::vector<std::string>();
        line = std::string(find.end(), line.end());
        boost::algorithm::trim(line);

        // Split at newlines
        std::vector<std::string> lines;
        boost::algorithm::split_regex(lines, line, boost::regex("\\\\N"));
        return lines;
    };

    // Parsing lambda
    auto parse = [&](AVSubtitle* sub, double ts_s, double ts_e) {
        c_media_subtitle::s_entry entry;
        entry.ts_s = ts_s;
        entry.ts_e = ts_e;
        //entry.ts_s = sub->start_display_time;
        //entry.ts_e = sub->end_display_time;
        for (unsigned int i = 0; i < sub->num_rects; i++) {
            AVSubtitleRect* rect = sub->rects[i];
            if (rect->type == SUBTITLE_TEXT && rect->text) {
                std::string line = rect->text;
                boost::algorithm::trim(line);
                if (!line.empty())
                    entry.content.push_back(line);
            } else if (rect->type == SUBTITLE_ASS && rect->ass) {
                std::string line = rect->ass;
                boost::algorithm::trim(line);
                std::vector<std::string> bits = parse_ass(line);
                for (auto bit : bits)
                    entry.content.push_back(bit);
            }
        }
        if (!entry.content.empty())
            m_entries.push_back(entry);
    };

    // Decoding lambda
    auto decode = [&](AVPacket* packet, int& got_sub) {
        // Used buffer size
        int decoded = packet->size;

        // Decode
        AVSubtitle sub;
        memset(&sub, 0, sizeof(sub));
        int ret = avcodec_decode_subtitle2(codec_context, &sub, &got_sub, packet);
        if (ret < 0)
            return ret;

        // Parse
        if (got_sub > 0) {
            //std::cout << boost::format("FFST: * pts = %d, dts = %d, duration = %d") % packet->pts % packet->dts % packet->duration << std::endl;
            double ts_s = timebase * packet->pts;
            double ts_e = ts_s + (timebase * packet->duration);
            parse(&sub, ts_s, ts_e);
            avsubtitle_free(&sub);
        }

        // Success
        return std::min(ret, decoded);
    };

    // Decoding loop
    AVPacket packet;
    av_init_packet(&packet);
    while (av_read_frame(format_context.get(), &packet) >= 0) {
        // Accept only subtitle stream
        if (packet.stream_index == stream) {
            AVPacket current = packet;
            do {
                int got_sub = 0;
                int ret = decode(&current, got_sub);
                if (ret < 0)
                    break;
                current.data += ret;
                current.size -= ret;
            } while (current.size > 0);
        }

        // Free packet
        av_packet_unref(&packet);
    }

    // Flush
    int got_sub = 0;
    packet.data = nullptr;
    packet.size = 0;
    do {
        decode(&packet, got_sub);
    } while (got_sub);
}
