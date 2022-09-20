#include "pch.h"
#include "Engine.h"
#include "MyExceptions.h"
#include "Helpers.h"

using Microsoft::WRL::ComPtr;

t::Engine::Engine() noexcept
{
}

void t::Engine::init(t::Context* context)
{
    m_context = context;

    // Initialize the WIC factory

    throw_if_failed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(m_wic_factory.GetAddressOf())));

    create_dd_resources(); // dd = device dependent
    create_swapchain_resources(); // resources that depend on the swap chain
}

void t::Engine::create_dd_resources()
{
    if (m_device)
    {
        return;
    }

    // Create the device and device context

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> device_context;

    const D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_0};

    throw_if_failed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, feature_levels, 1u, D3D11_SDK_VERSION, device.GetAddressOf(), nullptr, device_context.GetAddressOf()));
    throw_if_failed(device->QueryInterface(m_device.GetAddressOf()));
    throw_if_failed(device_context->QueryInterface(m_device_context.GetAddressOf()));

    // Fetch the DXGI factory

    ComPtr<IDXGIDevice1> dxgi_device;
    throw_if_failed(m_device->QueryInterface(dxgi_device.GetAddressOf()));

    ComPtr<IDXGIAdapter> adapter;
    throw_if_failed(dxgi_device->GetAdapter(adapter.GetAddressOf()));
    throw_if_failed(adapter->GetParent(IID_PPV_ARGS(m_dxgi_factory.GetAddressOf())));

    // Create the swap chain

    DXGI_SWAP_CHAIN_DESC1 desc1;
    desc1.Width = m_client_width;
    desc1.Height = m_client_height;
    desc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc1.Stereo = false;
    desc1.SampleDesc.Count = 1u;
    desc1.SampleDesc.Quality = 0u;
    desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc1.BufferCount = 2u;
    desc1.Scaling = DXGI_SCALING_STRETCH;
    desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    desc1.Flags = 0u;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen_desc;
    fullscreen_desc.RefreshRate.Numerator = 60u;
    fullscreen_desc.RefreshRate.Denominator = 1u;
    fullscreen_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    fullscreen_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    fullscreen_desc.Windowed = true;

    throw_if_failed(m_dxgi_factory->CreateSwapChainForHwnd(m_device.Get(), m_window, &desc1, &fullscreen_desc, nullptr, m_swap_chain.GetAddressOf()));

    // Load vertex shader bytecode

    Mallocker<uint8_t> vertex_shader_bytecode(load_binary_file("Main.VS.cso"));

    // Create input layout

    constexpr D3D11_INPUT_ELEMENT_DESC input_elems_desc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    constexpr D3D11_INPUT_ELEMENT_DESC input_elems_desc2[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    throw_if_failed(m_device->CreateInputLayout(input_elems_desc2, _countof(input_elems_desc2), vertex_shader_bytecode.data(), vertex_shader_bytecode.size(), m_input_layout.GetAddressOf()));

    // Create the ID3D11VertexShader

    throw_if_failed(m_device->CreateVertexShader(vertex_shader_bytecode.data(), vertex_shader_bytecode.size(), nullptr, m_vertex_shader.GetAddressOf()));

    // Create the ID3D11PixelShader

    Mallocker<uint8_t> pixel_shader_bytecode(load_binary_file("Main.PS.cso"));
    throw_if_failed(m_device->CreatePixelShader(pixel_shader_bytecode.data(), pixel_shader_bytecode.size(), nullptr, m_pixel_shader.GetAddressOf()));

    // Create the vertex buffer

    constexpr Vertex_data vertex_data[] =
    {
        // Data order: POSITION(FLOAT4), COLOR(FLOAT4)
        { {0.0f, 0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
        { {0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
        { {-0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} }
    };

    constexpr float vertex_data2[] =
    {
        // Data order: POSISTION(FLOAT4), TEXCOORD(FLOAT2)
        0.5f, 0.5f, 0.0f, 1.0f,    1.0f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, 1.0f,    1.0f, 1.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 1.0f,   0.0f, 1.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f,   0.0f, 0.0f  // topleft
    };

    constexpr float vertex_data3[] =
    {
        // Data order: POSISTION(FLOAT4), TEXCOORD(FLOAT2)
        0.5f,  0.5f, 0.0f, 1.0f,      1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f,      1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f,      0.0f, 0.0f,

        0.5f, -0.5f, 0.0f, 1.0f,      1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 1.0f,     0.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f,      0.0f, 0.0f
    };

    constexpr float vertex_data4[] =
    {
        // Data order: POSITION(FLOAT4), COLOR(FLOAT4)
        //0.0f, 0.5f, 0.0f, 1.0f,     0.0f, 0.0f, 1.0f, 1.0f,
        //0.5f, -0.5f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,
        //-0.5f, -0.5f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f

        0.0f, 0.5f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f,    1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 1.0f,   0.0f, 1.0f
    };

    D3D11_BUFFER_DESC buffer_desc;
    buffer_desc.ByteWidth = sizeof(vertex_data3);
    buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0u;
    buffer_desc.MiscFlags = 0u;
    buffer_desc.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA subresource_data;
    subresource_data.pSysMem = vertex_data3;
    subresource_data.SysMemPitch = 0u;
    subresource_data.SysMemSlicePitch = 0u;

    throw_if_failed(m_device->CreateBuffer(&buffer_desc, &subresource_data, m_vertex_buffer.GetAddressOf()));

    // Create the index buffer

    constexpr uint32_t indices[] = {0, 1, 3, 1, 2, 3};

    D3D11_BUFFER_DESC index_buffer_desc;
    index_buffer_desc.ByteWidth = sizeof(indices);
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags = 0u;
    index_buffer_desc.MiscFlags = 0u;
    index_buffer_desc.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA index_buffer_subres_data;
    index_buffer_subres_data.pSysMem = indices;
    index_buffer_subres_data.SysMemPitch = 0u;
    index_buffer_subres_data.SysMemSlicePitch = 0u;

    throw_if_failed(m_device->CreateBuffer(&index_buffer_desc, &index_buffer_subres_data, m_index_buffer.GetAddressOf()));

    // Load and create a texture and ShaderResourceView

    load_texture_from_file(L"title.png", m_wic_factory.Get(), m_device.Get(), m_background.GetAddressOf(), m_background_view.GetAddressOf());

    // Create sampler

    D3D11_SAMPLER_DESC sampler_desc;
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.MipLODBias = 0.0f;
    sampler_desc.MaxAnisotropy = 1u;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampler_desc.BorderColor[0] = 0.0f;
    sampler_desc.BorderColor[1] = 0.0f;
    sampler_desc.BorderColor[2] = 0.0f;
    sampler_desc.BorderColor[3] = 1.0f;
    sampler_desc.MinLOD = 0.0f;
    sampler_desc.MaxLOD = 0.0f;

    m_device->CreateSamplerState(&sampler_desc, m_sampler.GetAddressOf());
}

void t::Engine::create_swapchain_resources()
{
    if (!m_swap_chain)
    {
        return;
    }

    // Create the render target view

    ComPtr<ID3D11Texture2D> back_buffer;
    throw_if_failed(m_swap_chain->GetBuffer(0, IID_PPV_ARGS(back_buffer.GetAddressOf())));
    throw_if_failed(m_device->CreateRenderTargetView(back_buffer.Get(), nullptr, m_back_buffer_view.GetAddressOf()));
}

void t::Engine::OnResize()
{
}

/*
░░░░░░░░░░░▄▄▀▀▀▀▀▀▀▀▄▄
░░░░░░░░▄▀▀░░░░░░░░░░░░▀▄▄
░░░░░░▄▀░░░░░░░░░░░░░░░░░░▀▄
░░░░░▌░░░░░░░░░░░░░▀▄░░░░░░░▀▀▄
░░░░▌░░░░░░░░░░░░░░░░▀▌░░░░░░░░▌
░░░▐░░░░░░░░░░░░▒░░░░░▌░░░░░░░░▐
░░░▌▐░░░░▐░░░░▐▒▒░░░░░▌░░░░░░░░░▌
░░▐░▌░░░░▌░░▐░▌▒▒▒░░░▐░░░░░▒░▌▐░▐
░░▐░▌▒░░░▌▄▄▀▀▌▌▒▒░▒░▐▀▌▀▌▄▒░▐▒▌░▌
░░░▌▌░▒░░▐▀▄▌▌▐▐▒▒▒▒▐▐▐▒▐▒▌▌░▐▒▌▄▐
░▄▀▄▐▒▒▒░▌▌▄▀▄▐░▌▌▒▐░▌▄▀▄░▐▒░▐▒▌░▀▄
▀▄▀▒▒▌▒▒▄▀░▌█▐░░▐▐▀░░░▌█▐░▀▄▐▒▌▌░░░▀
░▀▀▄▄▐▒▀▄▀░▀▄▀░░░░░░░░▀▄▀▄▀▒▌░▐
░░░░▀▐▀▄▒▀▄░░░░░░░░▐░░░░░░▀▌▐
░░░░░░▌▒▌▐▒▀░░░░░░░░░░░░░░▐▒▐
░░░░░░▐░▐▒▌░░░░▄▄▀▀▀▀▄░░░░▌▒▐
░░░░░░░▌▐▒▐▄░░░▐▒▒▒▒▒▌░░▄▀▒░▐
░░░░░░▐░░▌▐▐▀▄░░▀▄▄▄▀░▄▀▐▒░░▐
░░░░░░▌▌░▌▐░▌▒▀▄▄░░░░▄▌▐░▌▒░▐
░░░░░▐▒▐░▐▐░▌▒▒▒▒▀▀▄▀▌▐░░▌▒░▌
░░░░░▌▒▒▌▐▒▌▒▒▒▒▒▒▒▒▐▀▄▌░▐▒▒
*/

IWICImagingFactory* t::Engine::return_wic_factory()
{
    return m_wic_factory.Get();
}

ID3D11Device1* t::Engine::return_device()
{
    return m_device.Get();
}

ID3D11DeviceContext1* t::Engine::return_device_context()
{
    return m_device_context.Get();
}