#pragma once

#include "render/imgui/imgui_re.hpp"
#include <SDL.h>
#include <iostream>

class Input{
public:

    static bool keys[];

    bool quit = false;

    void update();

private:
};
