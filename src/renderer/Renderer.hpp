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
    GLuint rendered_texture;
    GLuint depth_texture;
    GLuint depth_renderbuffer;

    SDL_Window* window;
    SDL_GLContext glcontext;

    float time = 0;
    int screen_width;
    int screen_height;

    static Renderer* instance;
};
