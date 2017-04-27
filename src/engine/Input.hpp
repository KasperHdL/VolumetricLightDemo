#pragma once

#include "render/imgui/imgui_re.hpp"
#include <SDL.h>
#include <iostream>

class Input{
private:
    static const Uint8* _last;
    static const Uint8* _now;

public:

    bool quit = false;

    void update();

    static bool get_key_down(Uint8 key);
    static bool get_key_on_down(Uint8 key);

private:
};
