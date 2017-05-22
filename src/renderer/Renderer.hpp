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

    Shader* geom_shader;
    Shader* stencil_shader;
    Shader* light_shader;
    Shader* screen_shader;
    Shader* shadow_map_shader;
    Shader* debug_shader;

    GLuint framebuffer = 0;
    GLuint depth_framebuffer = 0;
    GLuint depth_renderbuffer = 0;
    GLuint post_framebuffer = 0;

    GLuint position_texture;
    GLuint normal_texture;
    GLuint color_texture;
    GLuint specular_texture;

    GLuint depth_texture;

    GLuint post_texture;

    SDL_Window* window;
    SDL_GLContext glcontext;

    float time = 0;

    int max_shadow_maps = 1;
    int shadow_width = 3000;
    int shadow_height = 3000;

    int screen_width;
    int screen_height;

    static Renderer* instance;
private:
    void _render_scene(Shader* shader);
};
