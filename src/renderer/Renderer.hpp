#pragma once

#include "../utils/FileLoader.hpp"
#include <SDL_video.h>
#include "glm/glm.hpp"

#include "impl/GL.hpp"

#include <iostream>
#include <vector>

#include "Shader.hpp"
#include "Mesh.hpp"

class Camera;
class DebugInterface;
class Renderer{
public:
    Renderer();
    ~Renderer();

    void initialize(SDL_Window* window, int screen_width, int screen_height);

    void render(float delta_time);

    DebugInterface* debug;
    Camera* camera;
    Shader* shader;
    Shader* screen_shader;

    GLuint framebuffer = 0;
    GLuint depth_renderbuffer;

    GLuint position_texture;
    GLuint normal_texture;
    GLuint color_texture;
    GLuint depth_texture;

    SDL_Window* window;
    SDL_GLContext glcontext;

    float time = 0;
    int screen_width;
    int screen_height;

    static Renderer* instance;
};
