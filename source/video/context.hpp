/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_CONTEXT
#define H_VIDEO_CONTEXT

// Internal
#include "config.hpp"
#include "opengl/opengl.hpp"

// CEGUI
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/System.h>
#include <CEGUI/CEGUI.h>

// Video context class
class c_video_context : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_video_context(SDL_Window* window) {
            // OpenGL
            m_context = SDL_GL_CreateContext(window);

            // CEGUI
            m_cegui = std::unique_ptr<c_cegui>(new c_cegui());

            CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(cegui_system().getResourceProvider());
            rp->setResourceGroupDirectory("schemes", MJ_DATA_PATH "/cegui/schemes/");
            rp->setResourceGroupDirectory("imagesets", MJ_DATA_PATH "/cegui/imagesets/");
            rp->setResourceGroupDirectory("fonts", MJ_DATA_PATH "/cegui/fonts/");
            rp->setResourceGroupDirectory("layouts", MJ_DATA_PATH "/cegui/layouts/");
            rp->setResourceGroupDirectory("looknfeels", MJ_DATA_PATH "/cegui/looknfeel/");
            rp->setResourceGroupDirectory("lua_scripts", MJ_DATA_PATH "/cegui/lua_scripts/");

            CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
            CEGUI::Font::setDefaultResourceGroup("fonts");
            CEGUI::Scheme::setDefaultResourceGroup("schemes");
            CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
            CEGUI::WindowManager::setDefaultResourceGroup("layouts");
            CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
        }

        ~c_video_context() {
            m_cegui.reset();
            SDL_GL_DeleteContext(m_context);
        }

        // Operations
        bool make_current(SDL_Window* window) {
            return SDL_GL_MakeCurrent(window, m_context) == 0;
        }

        CEGUI::OpenGL3Renderer& cegui_renderer() { return m_cegui->renderer(); }
        CEGUI::System& cegui_system() { return m_cegui->system(); }

    private:
        // Context
        SDL_GLContext m_context;

        // CEGUI
        class c_cegui : boost::noncopyable {
            public:
                // Constructor and destructor
                c_cegui() :
                    m_renderer(CEGUI::OpenGL3Renderer::create()),
                    m_system(CEGUI::System::create(m_renderer))
                { }
                ~c_cegui() {
                    CEGUI::System::destroy();
                    CEGUI::OpenGL3Renderer::destroy(m_renderer);
                }

                // Gets
                CEGUI::OpenGL3Renderer& renderer() { return m_renderer; }
                CEGUI::System& system() { return m_system; }

            private:
                // Objects
                CEGUI::OpenGL3Renderer& m_renderer;
                CEGUI::System& m_system;
        };
        std::unique_ptr<c_cegui> m_cegui;
};

#endif
