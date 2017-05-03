#pragma once

#include "renderer/imgui/imgui_impl_sdl_gl3.hpp"
#include <SDL.h>
#include <iostream>

class Input{
private:
    static bool _last[];
    static bool _now[];

public:

    bool quit = false;

    void update();

    static bool get_key_down(Uint8 key);
    static bool get_key_on_down(Uint8 key);

private:
};
