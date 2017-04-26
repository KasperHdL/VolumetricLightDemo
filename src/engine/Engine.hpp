#pragma once

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

//render
#include "render/RenderEngine.hpp"
#include "render/Camera.hpp"
#include "render/Mesh.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "render/imgui/imgui_re.hpp"


class Game;
class Engine{
private:
        
    Shader* _shader;
public:
    SDL_Window* window;
    Game* game;
    Input input;

    int screen_width;
    int screen_height;

    static DynamicPool<Entity> entities;
    static bool quit;

    float time;
    float delta_time;

    bool draw_debug = false;

    Engine(int screen_width, int screen_height);
    ~Engine();

    int initialize(Game* game);

    void update(float delta_time);

    void render();

};

