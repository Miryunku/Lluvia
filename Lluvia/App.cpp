#include "pch.h"
#include "App.h"
#include "Helpers.h"
#include "MyExceptions.h"
#include "VertexStructures.h"

// 안녕하세요 just to ensure that this file is encoded in UTF-8

using Microsoft::WRL::ComPtr;

t::App::App(HINSTANCE instance)
    : m_instance(instance),
    m_window(nullptr)
{

}

void t::App::initialize()
{
    // Register the main window class

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = s_wndproc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_instance;
    wc.hIcon = static_cast<HICON>(LoadImage(nullptr, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED));
    wc.hCursor = static_cast<HCURSOR>(LoadImage(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED));
    wc.lpszClassName = L"main-window";
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.hIconSm = static_cast<HICON>(LoadImage(nullptr, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED));

    if (!RegisterClassEx(&wc))
    {
        throw std::runtime_error("App-init: RegisterClassEx failed");
    }

    // Create the main window

    uint32_t dpi = GetDpiForWindow(GetDesktopWindow());
    RECT window_area;
    window_area.left = 0;
    window_area.top = 0;
    window_area.right = std::ceil(800.0f * (dpi / 96.0f));
    window_area.bottom = std::ceil(600.0f * (dpi / 96.0f));

    if (!AdjustWindowRectExForDpi(&window_area, WS_OVERLAPPEDWINDOW, false, 0u, dpi))
    {
        throw std::runtime_error("App-init: AdjustWindowRectExForDpi failed");
    }

    m_window = CreateWindowEx(0u, L"main-window", L"Lluvia (version 1.0.0)", WS_OVERLAPPEDWINDOW, 20, 20, window_area.right - window_area.left, window_area.bottom - window_area.top, nullptr, nullptr, m_instance, this);
    if (!m_window)
    {
        throw std::runtime_error("App-init: CreateWindowEx failed");
    }

    // Get the size of the client area

    RECT rc;
    GetClientRect(m_window, &rc);
    m_client_width = rc.right;
    m_client_height = rc.bottom;

    // Initialize the context

    m_context.window = m_window;
    m_context.client_width = rc.right;
    m_context.client_height = rc.bottom;

    m_context.engine = &m_engine;
    m_context.tex_manager = &m_tex_manager;

    // Initialize the texture manager

    m_tex_manager.init(&m_context);

    // Initialize the engine

    m_engine.init(&m_context);
}

void t::App::clean_up()
{

}

int64_t CALLBACK t::App::s_wndproc(HWND wnd, uint32_t msg, uint64_t wparam, int64_t lparam)
{
    App* app = nullptr;

    if (msg == WM_CREATE)
    {
        CREATESTRUCT* creation_info = reinterpret_cast<CREATESTRUCT*>(lparam);
        app = reinterpret_cast<App*>(creation_info->lpCreateParams);
        SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<int64_t>(app));

        return DefWindowProc(wnd, msg, wparam, lparam);
    }
    else
    {
        app = reinterpret_cast<App*>(GetWindowLongPtr(wnd, GWLP_USERDATA));
        if (app)
        {
            return app->wndproc(wnd, msg, wparam, lparam);
        }
        else
        {
            return DefWindowProc(wnd, msg, wparam, lparam);
        }
    }
}

int64_t CALLBACK t::App::wndproc(HWND wnd, uint32_t msg, uint64_t wparam, int64_t lparam)
{
    try
    {
        switch (msg)
        {
            case WM_ERASEBKGND:
            {
                return 1;
            }

            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                BeginPaint(wnd, &ps);

                render();

                EndPaint(wnd, &ps);
                return 0;
            }

            case WM_CLOSE:
            {
                DestroyWindow(wnd);
                return 0;
            }

            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }

            default:
                return DefWindowProc(wnd, msg, wparam, lparam);
        }
    }
    catch (const t::MyException& e)
    {
        PostQuitMessage(-300);
        return 0;
    }
    catch (const t::MyHrException& e)
    {
        PostQuitMessage(-400);
        return 0;
    }
    catch (const std::exception& e)
    {
        /*
        * Exceptions that my functions can thrown:
        * std::exception
        * std::bad_alloc
        * std::runtime_error
        * std::overflow_error
        */
        PostQuitMessage(-500);
        return 0;
    }
}

HWND t::App::get_window() const
{
    return m_window;
}


void t::App::render()
{
    // IA Stage

    uint32_t stride = sizeof(Vertex_data2);
    uint32_t offset = 0;
    m_device_context->IASetInputLayout(m_input_layout.Get());
    m_device_context->IASetVertexBuffers(0, 1, m_vertex_buffer.GetAddressOf(), &stride, &offset);
    m_device_context->IASetIndexBuffer(m_index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
    m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // VS Stage

    m_device_context->VSSetShader(m_vertex_shader.Get(), nullptr, 0);

    // RS Stage
    
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = m_client_width;
    viewport.Height = m_client_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    m_device_context->RSSetViewports(1, &viewport);

    // PS Stage

    m_device_context->PSSetShader(m_pixel_shader.Get(), nullptr, 0);
    m_device_context->PSSetShaderResources(0u, 1u, m_background_view.GetAddressOf());
    m_device_context->PSSetSamplers(0u, 1u, m_sampler.GetAddressOf());

    // OM Stage

    m_device_context->OMSetRenderTargets(1, m_back_buffer_view.GetAddressOf(), nullptr);

    // Render
    
    constexpr float clearing_color[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    m_device_context->ClearRenderTargetView(m_back_buffer_view.Get(), clearing_color);
    //m_device_context->DrawIndexed(6u, 0u, 0);
    m_device_context->Draw(6u, 0u);
    m_swap_chain->Present(1, 0);
}