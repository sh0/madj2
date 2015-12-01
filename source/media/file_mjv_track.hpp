/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_FILE_MJV_TRACK
#define H_MEDIA_FILE_MJV_TRACK

// Internal
#include "config.hpp"

// FFmpeg
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
}

// Data chunk
class c_file_chunk : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_file_chunk(uint64_t size) :
            m_size(size),
            m_data(new uint8_t[size + FF_INPUT_BUFFER_PADDING_SIZE])
        {
            memset(&m_data[size], 0, FF_INPUT_BUFFER_PADDING_SIZE);
        }

        // Gets
        uint64_t size() { return m_size; }
        uint8_t* data() { return m_data.get(); }

    private:
        // Data
        uint64_t m_size;
        std::unique_ptr<uint8_t[]> m_data;
};

// Track data
class c_file_track : boost::noncopyable
{
    public:
        // Constructor
        c_file_track(uint64_t count, const uint8_t* data, uint64_t maxsize, std::unique_ptr<uint8_t[]>&& index) :
            m_count(count), m_data(data), m_maxsize(maxsize), m_index(std::move(index)) { }

        // Gets
        uint64_t count() { return m_count; }
        std::shared_ptr<c_file_chunk> read(uint64_t id)
        {
            // Check
            if (id >= m_count)
                return nullptr;

            // Size and offset
            uint8_t* index = m_index.get();
            uint64_t size = 0;
            for (int i = 0; i < 3; i++)
                size = (size << 8) | index[(8 * id) + i];
            uint64_t offset = 0;
            for (int i = 0; i < 5; i++)
                offset = (offset << 8) | index[(8 * id) + i + 3];

            // Check
            if (offset + size > m_maxsize)
                return nullptr;

            // Buffer
            auto chunk = std::make_shared<c_file_chunk>(size);
            memcpy(chunk->data(), m_data + offset, size);
            return chunk;
        }

    private:
        // Frame info
        uint64_t m_count;

        // Data
        const uint8_t* m_data;
        uint64_t m_maxsize;

        // Index
        std::unique_ptr<uint8_t[]> m_index;
};

#endif
