/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "mj_global.h"
#include "vo_main.h"
#include "vo_screen.h"

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

            // Start coordinates
            int view_x = x * (view_w - 1);
            int view_y = y * (view_h - 1);

            // Final viewport size correction
            if (x == m_view_cols - 1)
                view_w += m_window_width - ((view_w - 1) * m_view_cols + 1);
            if (y == m_view_rows - 1)
                view_h += m_window_height - ((view_h - 1) * m_view_rows + 1);

            // View
            /*
            auto view = std::make_shared<c_video_view>(
                m_window_width, m_window_height,
                view_x, m_window_height - (view_y + view_h),
                view_x + view_w, m_window_height - view_y
            );
            m_view_list.push_back(view);
            */
        }
    }
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

    // Init screen drawing
    gl_init();

    // Viewport drawing
    //for (auto &view : m_view_list)
    //    view->draw();
    dbg_gl_check();

    // Swap buffers
    SDL_GL_SwapWindow(m_window);
    dbg_gl_check();
}

// Input events
/*
void c_video_screen::input_dispatch(c_input* mgr)
{
    // Queue check
    if (!gdk_events_pending())
        return;

    // Get events
    GdkEvent* event;
    while ((event = gdk_event_get()) != nullptr) {
        // Check type
        if (event->type == GDK_KEY_PRESS || event->type == GDK_KEY_RELEASE) {
            uint32_t keyval = 0;
            gdk_event_get_keyval(event, &keyval);
            char* keyname = g_utf8_strdown(gdk_keyval_name(keyval), -1);
            if (keyname) {
                input_event(
                    mgr, std::string(keyname),
                    (event->type == GDK_KEY_PRESS ? 1.0 : 0.0)
                );
                g_free(keyname);
            }
        } else if (event->type == GDK_DESTROY || event->type == GDK_DELETE) {
            msg_critical(
                boost::format("Screen (%1%): Window destroyed!") %
                m_name
            );
        }

        // Free
        gdk_event_free(event);
    }
}
*/

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
    glDisable(GL_TEXTURE_2D);

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

