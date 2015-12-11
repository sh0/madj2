/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "media/file_mp4.hpp"

// Boost
#include <boost/algorithm/string.hpp>

// Constructor and destructor
c_media_file_mp4::c_media_file_mp4(boost::filesystem::path path) :
    // Path
    m_path(path),
    // File
    m_stream(nullptr), m_movie(nullptr), m_track(nullptr),
    // Info
    m_frames(0), m_width(0), m_height(0),
    // Scaling
    m_swscaler(nullptr)
{
    // Debug
    std::cout << boost::format("MP4: Opening file! path = %1%") % path << std::endl;

    // Stream
    AP4_Result result = AP4_FileByteStream::Create(path.c_str(), AP4_FileByteStream::STREAM_MODE_READ, m_stream);
    if (AP4_FAILED(result))
        throw c_exception("MP4: Failed to open file!", { throw_format("error", result), throw_format("path", m_path) });

    // File
    m_file = std::unique_ptr<AP4_File>(new AP4_File(*m_stream));
    m_movie = m_file->GetMovie();

    // Parse tracks
    auto& track_list = m_movie->GetTracks();
    for (AP4_Cardinal i = 0; i < track_list.ItemCount(); i++) {
        // Obtain track
        AP4_Track* track = nullptr;
        track_list.Get(i, track);

        // Check type
        if (track->GetType() != AP4_Track::Type::TYPE_VIDEO)
            continue;

        // Success
        m_track = track;
    }
    if (!m_track)
        throw c_exception("MP4: Failed to find video tracks!", { throw_format("path", m_path) });

    // Track description
    AP4_SampleDescription* desc = m_track->GetSampleDescription(0);
    if (!desc)
        throw c_exception("MP4: Failed to get sample description!", { throw_format("path", m_path) });

    // Info
    m_frames = m_track->GetSampleCount();

    // Video description
    AP4_VideoSampleDescription* desc_video = AP4_DYNAMIC_CAST(AP4_VideoSampleDescription, desc);
    if (!desc_video)
        throw c_exception("MP4: Could not obtain video descriptor!", { throw_format("path", m_path) });

    // Debug
    #if 0
    char format_fourcc[5];
    AP4_FormatFourChars(format_fourcc, desc->GetFormat());
    std::cout << boost::format("MP4: * FOURCC:      %s") % format_fourcc << std::endl;
    std::cout << boost::format("MP4: * Format name: %s") % AP4_GetFormatName(desc->GetFormat()) << std::endl;
    std::cout << boost::format("MP4: * Width:       %d") % desc_video->GetWidth() << std::endl;
    std::cout << boost::format("MP4: * Height:      %d") % desc_video->GetHeight() << std::endl;
    std::cout << boost::format("MP4: * Depth:       %d") % desc_video->GetDepth() << std::endl;
    #endif

    // Info
    m_width = desc_video->GetWidth();
    m_height = desc_video->GetHeight();

    // Parameters
    AVCodecID codec_id = AV_CODEC_ID_NONE;
    AVPixelFormat codec_pix_fmt = AV_PIX_FMT_NONE;
    AVColorRange codec_color_range = AVCOL_RANGE_UNSPECIFIED;

    // Complex descriptors
    if (desc->GetType() == AP4_SampleDescription::TYPE_MPEG) {
        // MPEG descriptor
        AP4_MpegSampleDescription* desc_mpeg = AP4_DYNAMIC_CAST(AP4_MpegSampleDescription, desc);
        if (!desc_mpeg)
            throw c_exception("MP4: Could not obtain MPEG descriptor!", { throw_format("path", m_path) });

        // Debug
        #if 0
        std::cout << boost::format("MP4: * Stream type: %s") % desc_mpeg->GetStreamTypeString(desc_mpeg->GetStreamType()) << std::endl;
        std::cout << boost::format("MP4: * Object type: %s") % desc_mpeg->GetObjectTypeString(desc_mpeg->GetObjectTypeId()) << std::endl;
        std::cout << boost::format("MP4: * Max bitrate: %d") % desc_mpeg->GetMaxBitrate() << std::endl;
        std::cout << boost::format("MP4: * Avg bitrate: %d") % desc_mpeg->GetAvgBitrate() << std::endl;
        std::cout << boost::format("MP4: * Buffer size: %d") % desc_mpeg->GetBufferSize() << std::endl;
        auto& info = desc_mpeg->GetDecoderInfo();
        std::string buffer;
        for (unsigned int j = 0; j < info.GetDataSize(); j++)
            buffer += boost::str(boost::format("%02x") % static_cast<int>(info.GetData()[j]));
        std::cout << boost::format("MP4: * Decode info: %s") % buffer << std::endl;
        #endif

        // Codec type
        auto type_id = desc_mpeg->GetObjectTypeId();
        if (type_id == AP4_OTI_JPEG) {
            codec_id = AV_CODEC_ID_MJPEG;
            codec_color_range = AVCOL_RANGE_JPEG;
        } else if (type_id == AP4_OTI_JPEG2000) {
            codec_id = AV_CODEC_ID_JPEG2000;
            codec_color_range = AVCOL_RANGE_JPEG;
        } else if (type_id == AP4_OTI_MPEG4_VISUAL) {
            codec_id = AV_CODEC_ID_MPEG4;
        } else if (
            type_id == AP4_OTI_MPEG2_VISUAL_SIMPLE || type_id == AP4_OTI_MPEG2_VISUAL_MAIN ||
            type_id == AP4_OTI_MPEG2_VISUAL_SNR || type_id == AP4_OTI_MPEG2_VISUAL_SPATIAL ||
            type_id == AP4_OTI_MPEG2_VISUAL_HIGH || type_id == AP4_OTI_MPEG2_VISUAL_422
        ) {
            codec_id = AV_CODEC_ID_MPEG2VIDEO;
        } else if (type_id == AP4_OTI_MPEG1_VISUAL) {
            codec_id = AV_CODEC_ID_MPEG1VIDEO;
        } else {
            throw c_exception("MP4: Unsupported MPEG codec!", { throw_format("path", m_path) });
        }
    } else if (desc->GetType() == AP4_SampleDescription::TYPE_AVC) {
        // AVC descriptor
        //AP4_AvcSampleDescription* desc_avc = AP4_DYNAMIC_CAST(AP4_AvcSampleDescription, desc);

        // Codec type
        codec_id = AV_CODEC_ID_H264;
        codec_pix_fmt = AV_PIX_FMT_YUV420P;

        // Debug
        #if 0
        std::cout << boost::format("MP4: * AVC Profile:          %d (%s)") % static_cast<int>(desc_avc->GetProfile()) % AP4_AvccAtom::GetProfileName(desc_avc->GetProfile()) << std::endl;
        std::cout << boost::format("MP4: * AVC Profile Compat:   %x") % static_cast<int>(desc_avc->GetProfileCompatibility()) << std::endl;
        std::cout << boost::format("MP4: * AVC Level:            %d") % static_cast<int>(desc_avc->GetLevel()) << std::endl;
        std::cout << boost::format("MP4: * AVC NALU Length Size: %d") % static_cast<int>(desc_avc->GetNaluLengthSize()) << std::endl;

        auto& sps = desc_avc->GetSequenceParameters();
        std::vector<std::string> sps_list;
        for (unsigned int i = 0; i < sps.ItemCount(); i++) {
            auto& data = sps[i];
            std::string buffer;
            for (unsigned int j = 0; j < data.GetDataSize(); j++)
                buffer += boost::str(boost::format("%02x") % static_cast<int>(data.GetData()[j]));
            sps_list.push_back(buffer);
        }
        std::cout << boost::format("MP4: * AVC sequence params:  %s") % boost::algorithm::join(sps_list, ", ") << std::endl;

        auto& pps = desc_avc->GetPictureParameters();
        std::vector<std::string> pps_list;
        for (unsigned int i = 0; i < pps.ItemCount(); i++) {
            auto& data = pps[i];
            std::string buffer;
            for (unsigned int j = 0; j < data.GetDataSize(); j++)
                buffer += boost::str(boost::format("%02x") % static_cast<int>(data.GetData()[j]));
            pps_list.push_back(buffer);
        }
        std::cout << boost::format("MP4: * AVC picture params:   %s") % boost::algorithm::join(pps_list, ", ") << std::endl;

        AP4_String codec;
        desc_avc->GetCodecString(codec);
        std::cout << boost::format("MP4: * AVC Codecs:           %s") % codec.GetChars() << std::endl;
        #endif

    } else if (desc->GetType() == AP4_SampleDescription::TYPE_HEVC) {
        // HEVC descriptor
        //AP4_HevcSampleDescription* desc_hevc = AP4_DYNAMIC_CAST(AP4_HevcSampleDescription, desc);

        // Codec type
        codec_id = AV_CODEC_ID_HEVC;
        codec_pix_fmt = AV_PIX_FMT_YUV420P;

        // Debug
        #if 0
        std::cout << boost::format("MP4: * HEVC Profile Space:       %d (%s)") % static_cast<int>(desc_hevc->GetGeneralProfileSpace()) % AP4_HvccAtom::GetProfileName(desc_hevc->GetGeneralProfileSpace(), desc_hevc->GetGeneralProfile()) << std::endl;
        std::cout << boost::format("MP4: * HEVC Profile:             %d") % static_cast<int>(desc_hevc->GetGeneralProfile()) << std::endl;
        std::cout << boost::format("MP4: * HEVC Profile Compat:      %x") % static_cast<int>(desc_hevc->GetGeneralProfileCompatibilityFlags()) << std::endl;
        std::cout << boost::format("MP4: * HEVC Level:               %d.%d") % static_cast<int>(desc_hevc->GetGeneralLevel() / 30) % static_cast<int>((desc_hevc->GetGeneralLevel() % 30) / 3) << std::endl;
        std::cout << boost::format("MP4: * HEVC Tier:                %d") % static_cast<int>(desc_hevc->GetGeneralTierFlag()) << std::endl;
        std::cout << boost::format("MP4: * HEVC Chroma Format:       %d (%s)") % static_cast<int>(desc_hevc->GetChromaFormat()) % AP4_HvccAtom::GetChromaFormatName(desc_hevc->GetChromaFormat()) << std::endl;
        std::cout << boost::format("MP4: * HEVC Chroma Bit Depth:    %d") % static_cast<int>(desc_hevc->GetChromaBitDepth()) << std::endl;
        std::cout << boost::format("MP4: * HEVC Luma Bit Depth:      %d") % static_cast<int>(desc_hevc->GetLumaBitDepth()) << std::endl;
        std::cout << boost::format("MP4: * HEVC Average Frame Rate:  %d") % static_cast<int>(desc_hevc->GetAverageFrameRate()) << std::endl;
        std::cout << boost::format("MP4: * HEVC Constant Frame Rate: %d") % static_cast<int>(desc_hevc->GetConstantFrameRate()) << std::endl;
        std::cout << boost::format("MP4: * HEVC NALU Length Size:    %d") % static_cast<int>(desc_hevc->GetNaluLengthSize()) << std::endl;

        AP4_String codec;
        desc_hevc->GetCodecString(codec);
        std::cout << boost::format("MP4: * HEVC Codecs:              %s") % codec.GetChars() << std::endl;
        #endif

    } else {
        throw c_exception("MP4: File has unknown video stream type!", { throw_format("path", m_path) });
    }

    // Codec
    if (codec_id == AV_CODEC_ID_NONE)
        throw c_exception("MP4: Unknown codec id!", { throw_format("path", m_path) });
    AVCodec* codec = avcodec_find_decoder(codec_id);
    if (!codec)
        throw c_exception("MP4: Could not find decoder!", { throw_format("id", codec_id), throw_format("path", m_path) });

    // Context
    m_codec_context = std::shared_ptr<AVCodecContext>(
        avcodec_alloc_context3(codec),
        [](AVCodecContext* ctx){ avcodec_close(ctx); avcodec_free_context(&ctx); }
    );

    m_codec_context->refcounted_frames = 0;
    m_codec_context->extradata = nullptr;
    if (m_width > 0)
        m_codec_context->width = m_width;
    if (m_height > 0)
        m_codec_context->height = m_height;
    if (codec_pix_fmt != AV_PIX_FMT_NONE)
        m_codec_context->pix_fmt = codec_pix_fmt;
    if (codec_color_range != AVCOL_RANGE_UNSPECIFIED)
        m_codec_context->color_range = codec_color_range;

    if (avcodec_open2(m_codec_context.get(), codec, nullptr) < 0)
        throw c_exception("MP4: Could not open codec!", { throw_format("id", codec_id), throw_format("path", m_path) });

    if (codec_color_range != AVCOL_RANGE_UNSPECIFIED && m_codec_context->color_range != codec_color_range)
        std::cout << "MP4: Color range reverted by codec!" << std::endl;

    // Decoding frame
    m_codec_frame = std::shared_ptr<AVFrame>(
        av_frame_alloc(),
        [](AVFrame* frame) { av_frame_free(&frame); }
    );

    // Info
    std::cout << boost::format("MP4: width = %d, height = %d, frames = %d, duration = %dms") %
        m_width % m_height % m_frames % m_track->GetDurationMs() << std::endl;

    // Debug
    #if 0
    auto temp = read(0);
    if (temp)
        std::cout << boost::format("Got frame! width = %d, height = %d") % temp->width() % temp->height() << std::endl;
    #endif
}

c_media_file_mp4::~c_media_file_mp4()
{
    // File
    m_file.reset();

    // Stream
    if (m_stream)
        m_stream->Release();
}

// Read
std::shared_ptr<c_opengl_image> c_media_file_mp4::read(int64_t id)
{
    // Range check
    if (id < 0 || id >= m_frames) {
        std::cout << "MP4: Sample id out of range!" << std::endl;
        return nullptr;
    }

    // Data
    AP4_Sample sample;
    AP4_DataBuffer buffer;
    AP4_Result result = m_track->ReadSample(id, sample, buffer);
    if (AP4_FAILED(result)) {
        std::cout << "MP4: Failed to read sample!" << std::endl;
        return nullptr;
    }

    // Packet
    AVPacket packet;
    av_init_packet(&packet);
    packet.size = buffer.GetDataSize();
    packet.data = buffer.UseData();
    packet.stream_index = 0;
    packet.pts = packet.dts = 0;
    //packet.pts = id;
    //packet.dts = sample.GetDts();
    packet.flags |= AV_PKT_FLAG_KEY;

    // Decode
    int got_picture = 0;
    int decoded = avcodec_decode_video2(m_codec_context.get(), m_codec_frame.get(), &got_picture, &packet);
    if (got_picture <= 0 || decoded < 0) {
        std::cout << "MP4: Failed to decode sample!" << std::endl;
        return nullptr;
    }

    // Validate
    if (m_codec_frame->format < 0)
        m_codec_frame->format = m_codec_context->pix_fmt;
    if (m_codec_frame->width <= 0 || m_codec_frame->height <= 0 || m_codec_frame->format < 0) {
        std::cout << boost::format("MP4: Invalid sample! width = %d, height = %d, format = %d") % m_codec_frame->width % m_codec_frame->height % m_codec_frame->format << std::endl;
        return nullptr;
    }

    // Check if conversion is needed
    AVPixelFormat srcfmt = static_cast<AVPixelFormat>(m_codec_frame->format);
    AVPixelFormat dstfmt = PIX_FMT_RGB24;
    if (srcfmt == AV_PIX_FMT_YUVJ420P) {
        srcfmt = AV_PIX_FMT_YUV420P;
    } else if (srcfmt == AV_PIX_FMT_YUVJ422P) {
        srcfmt = AV_PIX_FMT_YUV422P;
    } else if (srcfmt == AV_PIX_FMT_YUVJ444P) {
        srcfmt = AV_PIX_FMT_YUV444P;
    } else if (srcfmt == AV_PIX_FMT_YUVJ440P) {
        srcfmt = AV_PIX_FMT_YUV440P;
    }

    // Target image
    auto image = std::make_shared<c_opengl_image>(
        c_opengl_image::e_type::rgb24,
        m_codec_frame->width, m_codec_frame->height
    );

    // Set up scaler
    m_swscaler = sws_getCachedContext(
        m_swscaler,
        m_codec_frame->width, m_codec_frame->height, srcfmt,
        m_codec_frame->width, m_codec_frame->height, dstfmt,
        SWS_POINT, nullptr, nullptr, nullptr
    );
    if (!m_swscaler) {
        std::cout << "MP4: Failed to initialize scaler!" << std::endl;
        return nullptr;
    }

    // Run conversion
    AVPicture dstpic;
    memset(&dstpic, 0, sizeof(dstpic));
    avpicture_fill(&dstpic, image->data(), dstfmt, m_codec_frame->width, m_codec_frame->height);
    sws_scale(
        m_swscaler,
        m_codec_frame->data, m_codec_frame->linesize, 0, m_codec_frame->height,
        dstpic.data, dstpic.linesize
    );

    //av_image_copy
    //avpicture_layout

    // Success
    return image;
}
