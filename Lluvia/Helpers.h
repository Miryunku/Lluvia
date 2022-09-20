#pragma once

#include "Mallocker.h"

namespace t
{
    t::Mallocker<uint8_t> load_binary_file(const char* filepath);
}