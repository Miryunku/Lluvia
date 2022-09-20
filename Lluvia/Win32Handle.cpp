#include "pch.h"
#include "Win32Handle.h"

Win32Handle::Win32Handle(HANDLE handle)
{
    if (!handle)
    {
        throw std::runtime_error("");
    }
}