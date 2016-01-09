/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "media/file_ffms.hpp"

// Constructor and destructor
c_media_file_ffms::c_media_file_ffms(boost::filesystem::path path) :
    // Path
    m_path(path),
    // File
    m_source(nullptr),
    // Info
    m_frames(0), m_rate(0), m_aspect(1), m_width(0), m_height(0)
{
    // Debug
    //std::cout << boost::format("FFMS: Opening file! path = %1%") % path << std::endl;

    // FFMS error
    m_fferr.Buffer = m_ffmsg.data();
    m_fferr.BufferSize = m_ffmsg.size();
    m_fferr.ErrorType = FFMS_ERROR_SUCCESS;
    m_fferr.SubType = FFMS_ERROR_SUCCESS;

    // Library
    FFMS_Init(0, 1);

    // Index
    FFMS_Index* index = nullptr;

    // Cached index
    auto path_index = m_path;
    path_index.replace_extension(".ffindex");
    if (boost::filesystem::is_regular_file(path_index)) {
        // Read index
        index = FFMS_ReadIndex(path_index.c_str(), &m_fferr);
        if (index) {
            // Check validity
            int result = FFMS_IndexBelongsToFile(index, m_path.c_str(), &m_fferr);
            if (result) {
                // Invalid index
                FFMS_DestroyIndex(index);
                index = nullptr;

                // Delete index file too
                boost::filesystem::remove(path_index);
            }
        }
    }

    // Create index
    if (!index) {
        // Indexer
        FFMS_Indexer* indexer = FFMS_CreateIndexer(m_path.c_str(), &m_fferr);
        if (!indexer)
            throw c_exception("FFMS: Could not create indexer!", { throw_format("path", m_path) });

        //index = FFMS_DoIndexing2(indexer, FFMS_IEH_ABORT, &m_fferr);
        index = FFMS_DoIndexing(indexer, 0, 0, nullptr, nullptr, FFMS_IEH_ABORT, nullptr, nullptr, &m_fferr);
        if (!index)
            throw c_exception("FFMS: Failed to index media!", { throw_format("path", m_path) });

        // Write index to file
        FFMS_WriteIndex(path_index.c_str(), index, &m_fferr);
    }

    // Track
    int track_id = FFMS_GetFirstTrackOfType(index, FFMS_TYPE_VIDEO, &m_fferr);
    if (track_id < 0) {
        FFMS_DestroyIndex(index);
        throw c_exception("FFMS: Failed to find any video tracks!", { throw_format("path", m_path) });
    }

    // Source
    m_source = FFMS_CreateVideoSource(m_path.c_str(), track_id, index, 1, FFMS_SEEK_NORMAL, &m_fferr);
    if (!m_source) {
        FFMS_DestroyIndex(index);
        throw c_exception("FFMS: Failed to create video source!", { throw_format("path", m_path) });
    }

    // Destroy index
    FFMS_DestroyIndex(index);
    index = nullptr;

    // Video properties
    const FFMS_VideoProperties* props = FFMS_GetVideoProperties(m_source);
    m_frames = props->NumFrames;
    if (props->FirstTime < props->LastTime && props->LastTime > 0.0)
        m_rate = (props->LastTime - props->FirstTime) / static_cast<double>(m_frames);
    else if (props->FPSNumerator != 0)
        m_rate = static_cast<double>(props->FPSNumerator) / static_cast<double>(props->FPSDenominator);
    if (props->SARNum != 0)
        m_aspect = static_cast<double>(props->SARNum) / static_cast<double>(props->SARDen);

    // First frame
    const FFMS_Frame* frame = FFMS_GetFrame(m_source, 0, &m_fferr);
    if (!frame)
        throw c_exception("FFMS: Failed to get first video frame!", { throw_format("path", m_path) });
    if (frame->ScaledWidth > 0)
        m_width = frame->ScaledWidth;
    else
        m_width = frame->EncodedWidth;
    if (frame->ScaledHeight > 0)
        m_height = frame->ScaledHeight;
    else
        m_height = frame->EncodedHeight;

    // Conversion
    int pixfmts[2];
    pixfmts[0] = FFMS_GetPixFmt("rgb24");
    pixfmts[1] = -1;
    if (FFMS_SetOutputFormatV2(m_source, pixfmts, frame->EncodedWidth, frame->EncodedHeight, FFMS_RESIZER_POINT, &m_fferr))
        throw c_exception("FFMS: Failed to set output format!", { throw_format("path", m_path) });

    // Info
    std::cout << boost::format("FFMS: width = %d, height = %d, frames = %d, rate = %.3f, aspect = %.3f") %
        m_width % m_height % m_frames % m_rate % m_aspect << std::endl;
}

c_media_file_ffms::~c_media_file_ffms()
{
    // Source
    if (m_source)
        FFMS_DestroyVideoSource(m_source);
}

// Read
std::shared_ptr<c_opengl_image> c_media_file_ffms::read(int64_t id)
{
    // Check range
    if (id < 0 || id >= m_frames) {
        std::cout << "FFMS: Sample id out of range!" << std::endl;
        return nullptr;
    }

    // Get frame
    const FFMS_Frame* frame = FFMS_GetFrame(m_source, id, &m_fferr);
    if (!frame) {
        std::cout << "FFMS: Failed to decode frame!" << std::endl;
        return nullptr;
    }

    // Validate
    if (frame->EncodedWidth <= 0 || frame->EncodedHeight <= 0 || frame->Linesize[0] <= 0 || !frame->Data[0]) {
        std::cout << boost::format("MP4: Invalid sample! width = %d, height = %d, stride = %d") %
            frame->EncodedWidth % frame->EncodedHeight % frame->Linesize[0] << std::endl;
        return nullptr;
    }

    // Sample aspect ratio
    float aspect = 1.0f;
    if (frame->ScaledWidth != frame->EncodedWidth || frame->ScaledHeight != frame->EncodedHeight)
        aspect = frame->ScaledWidth / frame->ScaledHeight;

    // Target image
    auto image = std::make_shared<c_opengl_image>(
        c_opengl_image::e_type::rgb24,
        frame->EncodedWidth, frame->EncodedHeight, aspect, frame->Linesize[0]
    );
    memcpy(image->data(), frame->Data[0], frame->EncodedHeight * frame->Linesize[0]);

    // Success
    return image;
}
