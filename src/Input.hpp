#pragma once

#include "renderer/imgui/imgui_impl_sdl_gl3.hpp"
#include <SDL.h>
#include <iostream>
#include "glm/glm.hpp"

class Input{
private:
    static bool _last[];
    static bool _now[];

    static glm::vec2 _last_mouse;
    static glm::vec2 _current_mouse;

public:

    bool quit = false;

    void update();

    static bool get_key_down(Uint8 key);
    static bool get_key_on_down(Uint8 key);

    static glm::vec2 get_mouse_delta();
    static glm::vec2 get_mouse_position();


private:
};
