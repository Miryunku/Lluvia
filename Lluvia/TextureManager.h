#pragma once

#include "pch.h"
#include "App.h" // for t::Context

namespace t
{
    struct Wic_Translate
    {
        WICPixelFormatGUID wic;
        DXGI_FORMAT dxgi;
    };

    struct Wic_Translate_Half
    {
        WICPixelFormatGUID not_supported;
        WICPixelFormatGUID supported;
    };

    t::Wic_Translate wic_to_dxgi(WICPixelFormatGUID pixel_format, bool* is_conversion_needed);

    uint32_t get_bits_per_pixel(WICPixelFormatGUID pixel_format, IWICImagingFactory* wic_factory);

    void load_texture_from_file(const wchar_t* filepath, IWICImagingFactory* wic_factory, ID3D11Device1* device, ID3D11Texture2D** texture2D, ID3D11ShaderResourceView** srv);

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

    struct UV_Coord
    {
        float u, v = 0.0f;
    };

    struct Sprite_sheet
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
        std::wstring m_name;
    };

    struct Sprite
    {
        t::UV_Coord m_uv_top_left, m_uv_top_right, m_uv_bottom_right, m_uv_bottom_left;
        std::wstring m_name;
    };

    struct Sprite_solo
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
        t::UV_Coord m_uv_top_left, m_uv_top_right, m_uv_bottom_right, m_uv_bottom_left;
        std::wstring m_name;
    };

    void init_sprite_solo_texcoords(t::Sprite_solo* sprite_solo);

    class Texture_manager {
    public:
        Texture_manager() noexcept;

        void init(t::Context* context);
        void Load_main_textures();
        //void Load_anime_textures(); // WebPs
        void Load_skin_textures();
    private:
        t::Context* m_context;

        t::Sprite_solo m_welcome_bg;
        t::Sprite_sheet m_main_atlas;

        t::Sprite_sheet m_skin_atlas;

        //Sprite_solo m_chart_selection_bg;
        //Sprite_solo m_BGI_replacement;
        //Sprite_solo m_fail;
        //Sprite_solo m_clear;
        // TODO: The other Sprite_solos (Background in the playfield screen, etc)

        std::vector<t::Sprite> m_main_sprites; // Chart selection, key-binding screen, options screen, etc.
        //std::vector<t::Sprite> m_anime_sprites;
        std::vector<t::Sprite> m_skin_sprites;
    };
}