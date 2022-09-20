#include "pch.h"
#include "CFile.h"

t::CFile::CFile(const char* filepath, const char* mode) noexcept
{
    fopen_s(&m_file, filepath, mode);
}

t::CFile::~CFile() noexcept
{
    fclose(m_file);
}

t::CFile::operator FILE* ()
{
    return m_file;
}

bool t::CFile::init_succeeded() noexcept
{
    return m_file != nullptr;
}

FILE* t::CFile::get() noexcept
{
    return m_file;
}

FILE** t::CFile::address() noexcept
{
    return &m_file;
}