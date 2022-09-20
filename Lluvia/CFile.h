#pragma once

#include "pch.h"

namespace t
{
    class CFile {
    public:
        CFile(const char* filepath, const char* mode) noexcept;
        ~CFile() noexcept;

        operator FILE* ();

        bool init_succeeded() noexcept;
        FILE* get() noexcept;
        FILE** address() noexcept;
    private:
        FILE* m_file;
    };
}
