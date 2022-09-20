#include "pch.h"
#include "Helpers.h"
#include "CFile.h"

t::Mallocker<uint8_t> t::load_binary_file(const char* filepath)
{
    t::CFile file(filepath, "rb");
    if (!file.init_succeeded())
    {
        throw std::runtime_error("CFile construction in load_binary_file failed");
    }

    fseek(file, 0, SEEK_END);
    size_t buffer_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    Mallocker<uint8_t> buffer(buffer_size);

    if (fread(buffer.data(), 1, buffer_size, file) != buffer_size)
    {
        throw std::runtime_error("fread failed in load_binary_file or it didn't read all it had to");
    }

    return buffer;
}