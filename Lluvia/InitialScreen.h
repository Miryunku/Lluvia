#pragma once

#include "ScreenStack.h"

namespace t
{
    class Initial_screen : t::Screen {
    public:
        Initial_screen(t::Screen_stack* stack, t::Context* context, t::Engine* engine);
    private:
    };
}