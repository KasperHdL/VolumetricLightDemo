#pragma once

#include <SDL_video.h>
#include "glm/glm.hpp"

#include "impl/GL.hpp"

#include <iostream>
#include <vector>

#include "../utils/FileLoader.hpp"
#include "../utils/AssetManager.hpp"

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Light.hpp"

class Camera;
class Entity;
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
    Shader* depth_shader;
    Shader* debug_shader;

    GLuint framebuffer = 0;
    GLuint depth_framebuffer = 0;
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
private:
    void _render_scene(Shader* shader);
};
