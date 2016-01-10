/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "global.hpp"
#include "../context.hpp"
#include "controller/controller.hpp"
#include "video/video.hpp"
#include "video/screen.hpp"
#include "video/scancode.hpp"

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
    m_window_fullscreen(fullscreen),
    // Timing
    m_time(0)
{
    // Debug
    std::cout <<
        boost::format("Screen (%s): id=%d, pos_x=%d, pos_y=%d, width=%d, height=%d, fullscreen=%s") %
        m_name % m_id % m_window_pos_x % m_window_pos_y % m_window_width % m_window_height %
        std::string(m_window_fullscreen ? "true" : "false") << std::endl;

    // SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
        throw c_exception("Screen: Failed to initalize SDL!", { throw_format("name", m_name), throw_format("error", SDL_GetError()) });

    // Attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

    // Create window
    std::string title = std::string("madj - ") + m_name;
    Uint32 flags =
        SDL_WINDOW_OPENGL | (m_window_fullscreen ? (SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP) : SDL_WINDOW_RESIZABLE);
    m_window = SDL_CreateWindow(title.c_str(), m_window_pos_x, m_window_pos_y, m_window_width, m_window_height, flags);
    if (m_window == nullptr)
        throw c_exception("Screen: Failed to create window!", { throw_format("name", m_name), throw_format("error", SDL_GetError()) });

    // OpenGL context
    m_context = c_global::video->context(m_window);
    gl_init();

    // Window settings
    SDL_DisableScreenSaver();
    if (SDL_GL_SetSwapInterval(-1) != 0) {
        // 0 = immediate, 1 = vertical retrace sync, -1 = late swap tearing
        //std::cout << "Screen: Failed to set swap interval to late swap tearing!" << std::endl;
        if (SDL_GL_SetSwapInterval(1) != 0)
            std::cout << "Screen: Failed to set swap interval to vsync!" << std::endl;
    }

    // Display mode
    SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
    if (SDL_GetWindowDisplayMode(m_window, &mode) == 0 && mode.refresh_rate != 0) {
        std::cout << "Screen: refresh_rate = " << mode.refresh_rate << std::endl;
    }

    // Rendering area
    SDL_GL_GetDrawableSize(m_window, &m_window_width, &m_window_height);
    CEGUI::Rectf area(CEGUI::Vector2f(0.0f, 0.0f), CEGUI::Sizef(m_window_width, m_window_height));
    m_context->cegui_renderer().getDefaultRenderTarget().setArea(area);
    m_context->cegui_renderer().getDefaultRenderTarget().activate();

    // Timing
    m_time = SDL_GetTicks() / 1000.f;

    // CEGUI
    m_cegui = std::unique_ptr<c_cegui>(new c_cegui(m_context));

    // Scheme
    CEGUI::SchemeManager::getSingleton().createFromFile("GWEN.scheme");

    // Defaults
    //m_cegui->context().setDefaultFont("OpenSans-8");
    m_cegui->context().setDefaultFont("DroidSansMono-10");
    //m_cegui->context().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
    //m_cegui->context().setDefaultTooltipType("TaharezLook/Tooltip");

    // Root window
    m_cegui_root = reinterpret_cast<CEGUI::DefaultWindow*>(CEGUI::WindowManager::getSingleton().loadLayoutFromFile("screen.layout"));
    m_cegui_root->setUsingAutoRenderingSurface(false);
    //m_cegui_client = dynamic_cast<CEGUI::DefaultWindow*>(m_cegui_root->getChildRecursive("TrackerClient"));
    m_cegui->context().setRootWindow(m_cegui_root);

    // Error check
    g_opengl_check();

    // Viewport check
    if (
        m_view_rows < 1 || m_view_cols < 1 ||
        m_view_rows > 10 || m_view_cols > 10
    ) {
        throw c_exception("Screen: Wrong number of view columns/rows specified!", { throw_format("name", m_name) });
    }
}

c_video_screen::~c_video_screen()
{
    // Views
    for (auto& view : m_view_list)
        m_cegui_root->removeChild(view->window());
    m_view_list.clear();

    // Context
    m_context->make_current(nullptr);
    m_context.reset();

    // Window
    SDL_DestroyWindow(m_window);
}

// Dispatch
void c_video_screen::dispatch_input(c_time_cyclic& timer)
{
    // Poll events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                c_global::context->kill();
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                {
                    // Parse event
                    SDL_Keysym key_sym = event.key.keysym;
                    std::string key_name = SDL_GetKeyName(key_sym.sym);
                    boost::algorithm::to_lower(key_name);
                    bool key_down = (event.type == SDL_KEYDOWN);

                    // Specific key names
                    switch (key_sym.sym) {
                        case SDLK_LCTRL:
                        case SDLK_RCTRL:
                            key_name = "ctrl";
                            break;
                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT:
                            key_name = "shift";
                            break;
                        case SDLK_LALT:
                        case SDLK_RALT:
                            key_name = "alt";
                            break;
                        case SDLK_LGUI:
                        case SDLK_RGUI:
                            key_name = "gui";
                            break;
                    }

                    // Send event to controller
                    bool handled = false;
                    if (key_sym.sym == SDLK_ESCAPE && key_down) {
                        c_global::context->kill();
                        handled = true;
                    } else if (key_sym.sym == SDLK_RETURN && (key_sym.mod & KMOD_ALT) != 0 && key_down) {
                        m_window_fullscreen = !m_window_fullscreen;
                        //m_context->cegui_renderer().grabTextures();
                        if (SDL_SetWindowFullscreen(m_window, (m_window_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0)) < 0)
                            std::cout << boost::format("Screen: Failed to resize window! %s") % SDL_GetError() << std::endl;
                        //m_context->cegui_renderer().restoreTextures();
                        handled = true;
                    } else {
                        handled = c_global::controller->input_keyboard(key_name, key_down);
                    }

                    // Send event to GUI
                    if (!handled) {
                        CEGUI::Key::Scan key_cegui = static_cast<CEGUI::Key::Scan>(g_convert_sdl_scancode_to_cegui(static_cast<int>(event.key.keysym.scancode)));
                        if (key_cegui != 0) {
                            if (event.type == SDL_KEYDOWN)
                                m_cegui->context().injectKeyDown(key_cegui);
                            else if (event.type == SDL_KEYUP)
                                m_cegui->context().injectKeyUp(key_cegui);
                        }
                    }
                }
                break;

            case SDL_TEXTINPUT:
                {
                    // Send event to GUI
                    auto text = g_convert_sdl_textinput_to_cegui(event.text.text);
                    for (auto c : text)
                        m_cegui->context().injectChar(c);
                }
                break;

            case SDL_MOUSEMOTION:
                {
                    m_cegui->context().injectMousePosition(event.motion.x, event.motion.y);
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                {
                    // Parse event
                    CEGUI::MouseButton button = CEGUI::MouseButton::NoButton;
                    switch (event.button.button) {
                        case SDL_BUTTON_LEFT: button = CEGUI::MouseButton::LeftButton; break;
                        case SDL_BUTTON_MIDDLE: button = CEGUI::MouseButton::MiddleButton; break;
                        case SDL_BUTTON_RIGHT: button = CEGUI::MouseButton::RightButton; break;
                        case SDL_BUTTON_X1: button = CEGUI::MouseButton::X1Button; break;
                        case SDL_BUTTON_X2: button = CEGUI::MouseButton::X2Button; break;
                    }
                    if (button == CEGUI::MouseButton::NoButton)
                        break;

                    // Send event to GUI
                    m_cegui->context().injectMousePosition(event.button.x, event.button.y);
                    if (event.type == SDL_MOUSEBUTTONDOWN)
                        m_cegui->context().injectMouseButtonDown(button);
                    else if (event.type == SDL_MOUSEBUTTONUP)
                        m_cegui->context().injectMouseButtonUp(button);
                }
                break;

            case SDL_MOUSEWHEEL:
                {
                    // Send event to GUI
                    m_cegui->context().injectMouseWheelChange(event.wheel.y);
                }
                break;

            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        {
                            // New size
                            SDL_GL_GetDrawableSize(m_window, &m_window_width, &m_window_height);

                            // Debug
                            //std::cout << boost::format("Screen: New rendering surface! width = %d, height = %d") % m_window_width % m_window_height << std::endl;

                            // Renderer
                            m_context->cegui_renderer().setDisplaySize(CEGUI::Sizef(m_window_width, m_window_height));
                        }
                        break;

                    case SDL_WINDOWEVENT_LEAVE:
                        {
                            m_cegui->context().injectMouseLeaves();
                        }
                        break;
                }
                break;
        }
    }
}

void c_video_screen::dispatch_render(c_time_cyclic& timer)
{
    // Acquire context
    if (!m_context->make_current(m_window)) {
        std::cout << boost::format("Screen (%1%): Failed to acquire OpenGL context!") % m_name << std::endl;
        return;
    }
    g_opengl_check();

    // Time pulses
    const float time_current = SDL_GetTicks() / 1000.f;
    const float time_elapsed = time_current - m_time;
    m_context->cegui_system().injectTimePulse(time_elapsed);
    m_cegui->context().injectTimePulse(time_elapsed);
    m_time = time_current;

    // Viewport drawing
    for (auto& view : m_view_list)
        view->dispatch(timer);
    g_opengl_check();

    // Init screen drawing
    gl_init();

    // CEGUI
    CEGUI::Rectf area(CEGUI::Vector2f(0.0f, 0.0f), CEGUI::Sizef(m_window_width, m_window_height));
    m_cegui->target().setArea(area);
    //m_cegui->target().activate();
    m_context->cegui_renderer().beginRendering();
    m_cegui->context().draw();
    m_context->cegui_renderer().endRendering();
    //m_cegui->target().deactivate();
    g_opengl_check();

    // Swap buffers
    SDL_GL_SwapWindow(m_window);
    g_opengl_check();
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

    // Clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Error check
    g_opengl_check();

    // Return
    return true;
}

// View
void c_video_screen::view_add(std::shared_ptr<c_video_view> view)
{
    // List
    m_view_list.push_back(view);

    // Window
    auto window = view->window();
    float block_w = 1.0f / m_view_rows;
    float block_h = 1.0f / m_view_cols;
    window->setPosition(CEGUI::UVector2(CEGUI::UDim(view->pos_x() * block_w, 1.0f), CEGUI::UDim(view->pos_y() * block_h, 1.0f)));
    window->setSize(CEGUI::USize(CEGUI::UDim(view->pos_w() * block_w, -2.0f), CEGUI::UDim(view->pos_h() * block_h, -2.0f)));
    m_cegui_root->addChild(window);
}
