#pragma once

#include "pch.h"
#include "App.h"

// 안녕하세요 just to ensure that this file is encoded in UTF-8

namespace t
{
    class Engine {
    public:
        Engine() noexcept;

        void init(t::Context* context);
        void create_dd_resources(); // dd = device dependent
        void create_swapchain_resources(); // resources that depend on the swapchain

        void OnResize();

        // Getters ↓

        IWICImagingFactory* return_wic_factory();
        ID3D11Device1* return_device();
        ID3D11DeviceContext1* return_device_context();

    private:
        t::Context* m_context = nullptr;

        Microsoft::WRL::ComPtr<IWICImagingFactory> m_wic_factory;

        Microsoft::WRL::ComPtr<ID3D11Device1> m_device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_device_context;

        Microsoft::WRL::ComPtr<IDXGIFactory2> m_dxgi_factory;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swap_chain;

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_back_buffer_view;

        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertex_shader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader;

        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_input_layout;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertex_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_index_buffer;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_background;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background_view;

        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
    };
}