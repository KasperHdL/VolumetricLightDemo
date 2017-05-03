#pragma once

#include "../utils/FileLoader.hpp"
#include <SDL_video.h>
#include "glm/glm.hpp"

#include "impl/GL.hpp"

#include <iostream>
#include <vector>

#include "Camera.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

class DebugInterface;
class Renderer{
public:
    Renderer();
    ~Renderer();

    void initialize(SDL_Window* window, int screen_width, int screen_height);


    void render();
    GLuint vertex_buffer_id;

    DebugInterface* debug;
    Camera* camera;
    Shader* shader;
    Mesh* mesh;

    static Renderer* instance;

    SDL_Window* window;
    SDL_GLContext glcontext;

    int screen_width;
    int screen_height;

};
