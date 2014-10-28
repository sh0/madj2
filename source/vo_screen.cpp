/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "mj_global.h"
#include "mj_context.h"
#include "vo_main.h"
#include "vo_screen.h"

// Boost
#include <boost/algorithm/string.hpp>

// Constructor and destructor
c_video_screen::c_video_screen(
    uint id, const std::string& name, const std::string& color,
    int view_cols, int view_rows,
    int pos_x, int pos_y, int width, int height, bool fullscreen
) :
    // Info
    m_id(id), m_name(name), m_color(color),
    // View
    m_view_cols(view_cols), m_view_rows(view_rows),
    // Window
    m_window_pos_x(pos_x), m_window_pos_y(pos_y),
    m_window_width(width), m_window_height(height),
    m_window_fullscreen(fullscreen)
{
    // Debug
    std::cout <<
        boost::format("Screen (%s): Creating screen! id=%d, pos_x=%d, pos_y=%d, width=%d, height=%d, fullscreen=%s") %
        m_name % m_id % m_window_pos_x % m_window_pos_y % m_window_width % m_window_height %
        std::string(m_window_fullscreen ? "true" : "false") << std::endl;

    // SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
        throw c_exception("Screen: Failed to initalize SDL!", { throw_format("name", m_name), throw_format("error", SDL_GetError()) });

    // Attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

    // Create window
    std::string title = std::string("madj - ") + m_name;
    Uint32 flags = SDL_WINDOW_OPENGL | (m_window_fullscreen ? SDL_WINDOW_BORDERLESS : 0);
    m_window = SDL_CreateWindow(title.c_str(), m_window_pos_x, m_window_pos_y, m_window_width, m_window_height, flags);
    if (m_window == nullptr)
        throw c_exception("Screen: Failed to create window!", { throw_format("name", m_name), throw_format("error", SDL_GetError()) });

    // OpenGL context
    m_context = c_global::video->context(m_window);
    gl_init();

    // Window settings
    SDL_DisableScreenSaver();
    if (SDL_GL_SetSwapInterval(0) != 0) // 0 = immediate, 1 = vertical retrace sync, -1 = late swap tearing
        std::cout << "Screen: Failed to set swap interval!" << std::endl;

    // CEGUI
    m_cegui = std::unique_ptr<c_cegui>(new c_cegui(m_context));

    CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");

    m_cegui_glc = reinterpret_cast<CEGUI::GridLayoutContainer*>(CEGUI::WindowManager::getSingletonPtr()->createWindow("GridLayoutContainer"));
    m_cegui_glc->setGridDimensions(m_view_cols, m_view_rows);
    m_cegui->context().setRootWindow(m_cegui_glc);

    // Error check
    dbg_gl_check();

    // Viewport check
    if (
        m_view_rows < 1 || m_view_cols < 1 ||
        m_view_rows > 10 || m_view_cols > 10
    ) {
        throw c_exception("Screen: Wrong number of view columns/rows specified!", { throw_format("name", m_name) });
    }

    // Create viewports
    for (int y = 0; y < m_view_rows; y++) {
        for (int x = 0; x < m_view_cols; x++) {

            // Width and height
            int view_w = (m_window_width + m_view_cols - 1) / m_view_cols;
            int view_h = (m_window_height + m_view_rows - 1) / m_view_rows;

            // Name
            std::string view_name;
            view_name += "view_" + std::to_string(m_id);
            view_name += "_" + std::to_string(x) + "x" + std::to_string(y);

            // View
            auto view = std::make_shared<c_video_view>(view_name, view_w, view_h);
            m_view_list.push_back(view);

            // Window
            m_cegui_glc->addChildToPosition(view->window(), x, y);
        }
    }

    // Refresh layout
    m_cegui_glc->layout();
}

c_video_screen::~c_video_screen()
{
    // Context
    m_context->make_current(nullptr);
    m_context.reset();

    // Window
    SDL_DestroyWindow(m_window);
}

// Dispatch
void c_video_screen::dispatch()
{
    // Acquire context
    if (!m_context->make_current(m_window)) {
        std::cout << boost::format("Screen (%1%): Failed to acquire OpenGL context!") % m_name << std::endl;
        return;
    }
    dbg_gl_check();

    // Poll events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                c_global::context->kill();
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                SDL_Keysym key_sym = event.key.keysym;
                std::string key_name = SDL_GetKeyName(key_sym.sym);
                boost::algorithm::to_lower(key_name);
                bool key_down = (event.type == SDL_KEYDOWN);
                bool key_ctrl = ((key_sym.mod & KMOD_CTRL) != 0);
                bool key_shift = ((key_sym.mod & KMOD_SHIFT) != 0);
                bool key_alt = ((key_sym.mod & KMOD_ALT) != 0);
                bool key_gui = ((key_sym.mod & KMOD_GUI) != 0);

                if (key_sym.sym == SDLK_ESCAPE)
                    c_global::context->kill();
                else
                    std::cout << "Screen: Key: " << key_name << std::endl;
                //else
                    //c_global::input->input_keyboard(key_name, key_down, key_ctrl, key_shift, key_alt, key_gui);
                break;
        }
    }

    // Init screen drawing
    gl_init();

    // Viewport drawing
    for (auto& view : m_view_list)
        view->dispatch();
    dbg_gl_check();

    // CEGUI
    CEGUI::Rectf area(CEGUI::Vector2f(0.0f, 0.0f), CEGUI::Sizef(m_window_width, m_window_height));
    m_cegui->target().setArea(area);
    m_cegui->target().activate();
    m_context->cegui_renderer().beginRendering();
    m_cegui->context().draw();
    m_context->cegui_renderer().endRendering();
    m_cegui->target().deactivate();

    // Swap buffers
    SDL_GL_SwapWindow(m_window);
    dbg_gl_check();
}

// GL functions
bool c_video_screen::gl_init()
{
    // Viewport
    glViewport(0, 0, m_window_width, m_window_height);

    // Vertex properties
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    // Fragment properties
    glDisable(GL_BLEND);
    //glDisable(GL_TEXTURE_2D);

    // Clear
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Antialias
    glShadeModel(GL_SMOOTH);

    // Error check
    dbg_gl_check();

    // Return
    return true;
}

