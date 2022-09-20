#include "pch.h"
#include "TextureManager.h"
#include "MyExceptions.h"
#include "Mallocker.h"

using Microsoft::WRL::ComPtr;

namespace
{
    t::Wic_Translate wic_dxgi_table[] =
    {
        // Directly supported formats, as in, WIC -> DXGI
        {GUID_WICPixelFormat128bppRGBAFloat, DXGI_FORMAT_R32G32B32A32_FLOAT},
        {GUID_WICPixelFormat64bppRGBAHalf, DXGI_FORMAT_R16G16B16A16_FLOAT},
        {GUID_WICPixelFormat64bppRGBA, DXGI_FORMAT_R16G16B16A16_UNORM},
        {GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM},
        {GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM}, // DXGI 1.1
        {GUID_WICPixelFormat32bppBGR, DXGI_FORMAT_B8G8R8X8_UNORM}, // DXGI 1.1
        {GUID_WICPixelFormat32bppRGBA1010102XR, DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM}, // DXGI 1.1
        {GUID_WICPixelFormat32bppRGBA1010102, DXGI_FORMAT_R10G10B10A2_UNORM},
        {GUID_WICPixelFormat32bppRGBE, DXGI_FORMAT_R9G9B9E5_SHAREDEXP},
        {GUID_WICPixelFormat16bppBGRA5551, DXGI_FORMAT_B5G5R5A1_UNORM}, // ifdef DXGI_1_2_FORMATS
        {GUID_WICPixelFormat16bppBGR565, DXGI_FORMAT_B5G6R5_UNORM}, // ifdef DXGI_1_2_FORMATS
        {GUID_WICPixelFormat32bppGrayFloat, DXGI_FORMAT_R32_FLOAT},
        {GUID_WICPixelFormat16bppGrayHalf, DXGI_FORMAT_R16_FLOAT},
        {GUID_WICPixelFormat16bppGray, DXGI_FORMAT_R16_UNORM},
        {GUID_WICPixelFormat8bppGray, DXGI_FORMAT_R8_UNORM},
        {GUID_WICPixelFormat8bppAlpha, DXGI_FORMAT_A8_UNORM},
        {GUID_WICPixelFormat96bppRGBFloat, DXGI_FORMAT_R32G32B32_FLOAT} // _WIN32_WINNT >= 0x0602
    };

    t::Wic_Translate_Half wic_wic_table[] =
    {
        // Not directly supported formats
        {GUID_WICPixelFormatBlackWhite, GUID_WICPixelFormat8bppGray},

        {GUID_WICPixelFormat1bppIndexed, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat2bppIndexed, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat4bppIndexed, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat8bppIndexed, GUID_WICPixelFormat32bppRGBA},

        {GUID_WICPixelFormat2bppGray, GUID_WICPixelFormat8bppGray},
        {GUID_WICPixelFormat4bppGray, GUID_WICPixelFormat8bppGray},

        {GUID_WICPixelFormat16bppGrayFixedPoint, GUID_WICPixelFormat16bppGrayHalf},
        {GUID_WICPixelFormat32bppGrayFixedPoint, GUID_WICPixelFormat32bppGrayFloat},

        {GUID_WICPixelFormat16bppBGR555, GUID_WICPixelFormat16bppBGRA5551}, // ifdef DXGI_1_2_FORMATS

        {GUID_WICPixelFormat32bppBGR101010, GUID_WICPixelFormat32bppRGBA1010102},

        {GUID_WICPixelFormat24bppBGR, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat24bppRGB, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat32bppPBGRA, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat32bppPRGBA, GUID_WICPixelFormat32bppRGBA},

        {GUID_WICPixelFormat48bppRGB, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat48bppBGR, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat64bppBGRA, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat64bppPRGBA, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat64bppPBGRA, GUID_WICPixelFormat64bppRGBA},

        {GUID_WICPixelFormat48bppRGBFixedPoint, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat48bppBGRFixedPoint, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat64bppRGBAFixedPoint, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat64bppBGRAFixedPoint, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat64bppRGBFixedPoint, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat64bppRGBHalf, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat48bppRGBHalf, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat16bppBGRA5551, GUID_WICPixelFormat64bppRGBAHalf},

        {GUID_WICPixelFormat96bppRGBFixedPoint, GUID_WICPixelFormat128bppRGBAFloat},
        {GUID_WICPixelFormat128bppPRGBAFloat, GUID_WICPixelFormat128bppRGBAFloat},
        {GUID_WICPixelFormat128bppRGBFloat, GUID_WICPixelFormat128bppRGBAFloat},
        {GUID_WICPixelFormat128bppRGBAFixedPoint, GUID_WICPixelFormat128bppRGBAFloat},
        {GUID_WICPixelFormat128bppRGBFixedPoint, GUID_WICPixelFormat128bppRGBAFloat},

        {GUID_WICPixelFormat32bppCMYK, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat64bppCMYK, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat40bppCMYKAlpha, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat80bppCMYKAlpha, GUID_WICPixelFormat64bppRGBA},

        {GUID_WICPixelFormat32bppRGB, GUID_WICPixelFormat32bppRGBA}, // _WIN32_WINNT >= 0x0602
        {GUID_WICPixelFormat64bppRGB, GUID_WICPixelFormat64bppRGBA}, // _WIN32_WINNT >= 0x0602
        {GUID_WICPixelFormat64bppPRGBAHalf, GUID_WICPixelFormat64bppRGBAHalf} // _WIN32_WINNT >= 0x0602

        // n-channel formats not supported
    };
}

t::Wic_Translate t::wic_to_dxgi(WICPixelFormatGUID pixel_format, bool* is_conversion_needed)
{
    // Search the WICPixelFormat in the wic_dxgi table, if found it means that the image
    // already has a pixel format directly compatible with a DXGI format, and we need to
    // know which DXGI format that is.

    for (int32_t i = 0; i < 17; ++i) // 17 = number of elements in wic_dxgi_table
    {
        if (memcmp(&pixel_format, &wic_dxgi_table[i].wic, sizeof(WICPixelFormatGUID)) == 0)
        {
            return {pixel_format, wic_dxgi_table[i].dxgi};
        }
    }

    // If we reached here, it means that the WICPixelFormat of the image is not directly
    // compatible with a DXGI format, so we will try to see if the WICPixelFormat of the
    // image can be converted to another WICPixelFormat that is directly compatible with
    // a DXGI format.

    for (int32_t i = 0; i < 40; ++i) // 40 = number of elements in wic_wic_table
    {
        if (memcmp(&pixel_format, &wic_wic_table[i].not_supported, sizeof(WICPixelFormatGUID)) == 0)
        {
            WICPixelFormatGUID supported = wic_wic_table[i].supported;
            for (int32_t j = 0; j < 17; ++j)
            {
                if (memcmp(&supported, &wic_dxgi_table[j].wic, sizeof(WICPixelFormatGUID)) == 0)
                {
                    *is_conversion_needed = true;
                    return {supported, wic_dxgi_table[j].dxgi};
                }
            }
        }
    }

    return {GUID_WICPixelFormatUndefined, DXGI_FORMAT_UNKNOWN};
}

uint32_t t::get_bits_per_pixel(WICPixelFormatGUID pixel_format, IWICImagingFactory* wic_factory)
{
    ComPtr<IWICComponentInfo> component_info;
    ComPtr<IWICPixelFormatInfo> pixel_format_info;
    uint32_t bpp = 0;

    throw_if_failed(wic_factory->CreateComponentInfo(pixel_format, component_info.GetAddressOf()));
    throw_if_failed(component_info->QueryInterface(IID_PPV_ARGS(pixel_format_info.GetAddressOf())));
    throw_if_failed(pixel_format_info->GetBitsPerPixel(&bpp));

    return bpp;
}

void t::load_texture_from_file(const wchar_t* filepath, IWICImagingFactory* wic_factory, ID3D11Device1* device, ID3D11Texture2D** texture2D, ID3D11ShaderResourceView** srv)
{
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    WICPixelFormatGUID pixel_format;

    throw_if_failed(wic_factory->CreateDecoderFromFilename(filepath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf()));
    throw_if_failed(decoder->GetFrame(0, frame.GetAddressOf()));
    throw_if_failed(frame->GetPixelFormat(&pixel_format));

    uint32_t bpp = get_bits_per_pixel(pixel_format, wic_factory);
    uint32_t image_width;
    uint32_t image_height;
    throw_if_failed(frame->GetSize(&image_width, &image_height));
    // The total size in bytes of the image is used to allocate enough memory
    uint64_t bytewidth = (image_width * bpp + 7u) / 8u;
    uint64_t total_bytes = bytewidth * image_height;
    if (bytewidth > UINT32_MAX || total_bytes > UINT32_MAX)
    {
        throw LLV_EXCEPTION(L"overflow_error");
    }
    Mallocker<uint8_t> buffer(total_bytes);

    bool conversion_needed = false;
    Wic_Translate wic_translate = wic_to_dxgi(pixel_format, &conversion_needed);
    if (wic_translate.dxgi != DXGI_FORMAT_UNKNOWN)
    {
        if (conversion_needed)
        {
            ComPtr<IWICFormatConverter> format_converter;
            throw_if_failed(wic_factory->CreateFormatConverter(format_converter.GetAddressOf()));
            throw_if_failed(format_converter->Initialize(frame.Get(), wic_translate.wic, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom));
            throw_if_failed(format_converter->CopyPixels(nullptr, bytewidth, total_bytes, buffer.data()));
        }
        else
        {
            throw_if_failed(frame->CopyPixels(nullptr, bytewidth, total_bytes, buffer.data()));
        }
    }
    else
    {
        throw LLV_EXCEPTION(L"WICPixelFormat not supported in load_texture_from_file");
    }

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1u;
    desc.ArraySize = 1u;
    desc.Format = wic_translate.dxgi;
    desc.SampleDesc.Count = 1u;
    desc.SampleDesc.Quality = 0u;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0u;
    desc.MiscFlags = 0u;

    D3D11_SUBRESOURCE_DATA subres_desc;
    subres_desc.pSysMem = buffer.data();
    subres_desc.SysMemPitch = bytewidth;
    subres_desc.SysMemSlicePitch = total_bytes;

    throw_if_failed(device->CreateTexture2D(&desc, &subres_desc, texture2D));

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    srv_desc.Format = wic_translate.dxgi;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0u;
    srv_desc.Texture2D.MipLevels = 1u;

    throw_if_failed(device->CreateShaderResourceView(*texture2D, &srv_desc, srv));
}

void t::init_sprite_solo_texcoords(t::Sprite_solo* sprite_solo)
{
    assert(sprite_solo != nullptr);

    sprite_solo->m_uv_top_left.u = 0.0f;
    sprite_solo->m_uv_top_left.v = 0.0f;

    sprite_solo->m_uv_top_right.u = 1.0f;
    sprite_solo->m_uv_top_right.v = 0.0f;

    sprite_solo->m_uv_bottom_right.u = 1.0f;
    sprite_solo->m_uv_bottom_right.v = 1.0f;

    sprite_solo->m_uv_bottom_left.u = 0.0f;
    sprite_solo->m_uv_bottom_left.v = 1.0f;
}

/*
       lllllllllllllllllllllll
      lllllllllllllllllllllllll
     llllllllll       llllllllll
    llllllllll         llllllllll
   lllllllllllllllllllllllllllllll
  lllllllllllllllllllllllllllllllll
 llllllllll               llllllllll
llllllllll                 llllllllll
*/

t::Texture_manager::Texture_manager() noexcept
{
}

void t::Texture_manager::init(t::Context* context)
{
    m_context = context;
}

void t::Texture_manager::Load_main_textures()
{
    // Load welcome image

    IWICImagingFactory* wic_factory = m_context->engine->return_wic_factory();
    ID3D11Device1* device = m_context->engine->return_device();

    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> texture_view;

    t::load_texture_from_file(L"Skinning/InitialScreen.png", wic_factory, device, texture.GetAddressOf(), texture_view.GetAddressOf());

    m_welcome_bg.m_texture = std::move(texture);
    m_welcome_bg.m_srv = std::move(texture_view);
    t::init_sprite_solo_texcoords(&m_welcome_bg);
    m_welcome_bg.m_name = L"WelcomeImage";

    // Load main textures


}

void t::Texture_manager::Load_skin_textures()
{
}