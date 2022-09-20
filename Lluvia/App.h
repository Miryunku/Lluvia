#pragma once

#include "pch.h"
#include "Engine.h"
#include "TextureManager.h"

// Dxgi1_5.lib
// DXGI.lib

namespace t
{
    struct Context // the pointers are non-owning
    {
        HWND window = nullptr;
        uint32_t client_width = 0u;
        uint32_t client_height = 0u;

        t::Engine* engine = nullptr;
        t::Texture_manager* tex_manager = nullptr;
    };

    class App {
    public:
        App(HINSTANCE instance);
        void initialize();
        void clean_up();

        HWND get_window() const;

        static int64_t CALLBACK s_wndproc(HWND wnd, uint32_t msg, uint64_t wparam, int64_t lparam);
        int64_t CALLBACK wndproc(HWND wnd, uint32_t msg, uint64_t wparam, int64_t lparam);
    private:
        HINSTANCE m_instance;
        HWND m_window;

        uint32_t m_client_width = 0;
        uint32_t m_client_height = 0;

        t::Context m_context;
        t::Engine m_engine;
        t::Texture_manager m_tex_manager;
    };
}
