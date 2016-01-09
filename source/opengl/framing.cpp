/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "opengl/framing.hpp"

// Box frames
std::array<float, 4> c_opengl_framing::zoombox_quad(std::shared_ptr<c_opengl_texture_2d> src, std::shared_ptr<c_opengl_texture_2d> dst)
{
    // x1, y1, x2, y2
    float src_aspect = src->aspect() * static_cast<float>(src->width()) / static_cast<float>(src->height());
    float dst_aspect = dst->aspect() * static_cast<float>(dst->width()) / static_cast<float>(dst->height());
    if (src_aspect >= dst_aspect) {
        float r = ((src_aspect / dst_aspect) - 1.0f) / 2.0f;
        float x1 = r;
        float x2 = 1.0f - r;
        return {{ x1, 0.0f, x2, 1.0f }};
    } else {
        float r = ((dst_aspect / src_aspect) - 1.0f) / 2.0f;
        float y1 = r;
        float y2 = 1.0f - r;
        return {{ 0.0f, y1, 1.0f, y2 }};
    }
    return {{ 0.0f, 0.0f, 1.0f, 1.0f }};
}

std::array<float, 4> c_opengl_framing::letterbox_quad(std::shared_ptr<c_opengl_texture_2d> src, std::shared_ptr<c_opengl_texture_2d> dst)
{
    // x1, y1, x2, y2
    float src_aspect = src->aspect() * static_cast<float>(src->width()) / static_cast<float>(src->height());
    float dst_aspect = dst->aspect() * static_cast<float>(dst->width()) / static_cast<float>(dst->height());
    if (src_aspect >= dst_aspect) {
        float r = ((src_aspect / dst_aspect) - 1.0f) / 2.0f;
        float x1 = r;
        float x2 = 1.0f - r;
        return {{ x1, 0.0f, x2, 1.0f }};
    } else {
        float r = ((dst_aspect / src_aspect) - 1.0f) / 2.0f;
        float y1 = r;
        float y2 = 1.0f - r;
        return {{ 0.0f, y1, 1.0f, y2 }};
    }
    return {{ 0.0f, 0.0f, 1.0f, 1.0f }};
}
