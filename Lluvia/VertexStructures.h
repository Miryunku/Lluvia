#pragma once

#include <DirectXMath.h>

namespace t
{
    struct Vertex_data
    {
        DirectX::XMFLOAT4 position;
        DirectX::XMFLOAT4 color;
    };

    struct Vertex_data2
    {
        DirectX::XMFLOAT4 position;
        DirectX::XMFLOAT2 texcoord;
    };
}
