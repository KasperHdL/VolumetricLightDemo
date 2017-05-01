#pragma once

#include "../utils/FileLoader.hpp"
#include <SDL_video.h>
#include "glm/glm.hpp"

#include "impl/GL.hpp"

#include <iostream>
#include <vector>

class Renderer{
public:
    Renderer();
    ~Renderer();

    void initialize(SDL_Window* window, int screen_width, int screen_height);


    void render();

    unsigned int compile_shader(const char* vertex_shader, const char* fragment_shader);

    static Renderer* instance;

    SDL_Window* window;
    SDL_GLContext glcontext;

    int screen_width;
    int screen_height;

};
