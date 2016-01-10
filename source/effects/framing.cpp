/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "global.hpp"
#include "video/video.hpp"
#include "effects/framing.hpp"
#include "opengl/framing.hpp"

// Constructor
c_effects_zoombox::c_effects_zoombox()
{
    m_program = c_global::video->shader()->program("effect-zoombox");
}

// Execute
void c_effects_zoombox::process(std::shared_ptr<c_opengl_texture_2d> src, std::shared_ptr<c_opengl_texture_2d> dst)
{
    // Uniforms and attributes
    auto u_texture = m_program->uniform("u_texture");
    BOOST_ASSERT(u_texture);
    auto u_framing = m_program->uniform("u_framing");
    BOOST_ASSERT(u_framing);
    auto m_vecpos = m_program->attribute("m_vecpos");
    BOOST_ASSERT(m_vecpos);

    // Texture
    src->bind(0);
    u_texture->set_s(0);

    // Coordinates
    auto framing = c_opengl_framing::zoombox_quad(src, dst);
    u_framing->set_f4(framing[0], framing[1], framing[2], framing[3]);

    // Render
    m_program->use_begin();

    m_program->use_end();

    // Texture
    src->unbind();
}
