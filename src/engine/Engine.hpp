#pragma once

//
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>


//sdl
#include "SDL.h"

//glm
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

//engine
#include "Entity.hpp"
#include "Input.hpp"
#include "utils/DynamicPool.hpp"
#include "utils/FileLoader.hpp"
//render
#include "render/RenderEngine.hpp"
#include "render/Camera.hpp"
#include "render/Mesh.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "render/imgui/imgui_re.hpp"


class Game;
class DebugInterface;

class Engine{
private:
    Shader* _shader = nullptr;
    int _shader_time;
public:
    SDL_Window* window;
    Game* game;
    Input input;
    DebugInterface* debug;

    int screen_width;
    int screen_height;

    static DynamicPool<Entity> entities;
    static Camera* camera;

    float time;
    float delta_time;

    Engine(int screen_width, int screen_height);
    ~Engine();

    int initialize(Game* game);

    void update(float delta_time);

    void render(float delta_time);

    void update_shaders();

};

