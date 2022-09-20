#pragma once

// WinAPI

#define NOMINMAX

#define NOGDI
#define NODRAWTEXT
#define NOBITMAP

#define NOHELP

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <wrl/client.h>

// WIC

#include <wincodec.h>

// Direct3D 11

#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

// C/C++ Standard Library

#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <memory>
#include <vector>
#include <string>