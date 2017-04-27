
#include "Engine.hpp"

#include "../game/Game.hpp"
#include "utils/DebugInterface.hpp"



DynamicPool<Entity> Engine::entities = DynamicPool<Entity>(32);
bool Engine::quit = false;

Engine::Engine(int screen_width, int screen_height){
    this->screen_width = screen_width;
    this->screen_height = screen_height;
}

Engine::~Engine(){
}

int Engine::initialize(Game* game){
    this->game = game;

    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
            "An SDL2 window",                     // window title
            SDL_WINDOWPOS_UNDEFINED,              // initial x position
            SDL_WINDOWPOS_UNDEFINED,              // initial y position
            screen_width,                                  // width, in pixels
            screen_height,                                  // height, in pixels
            SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE// flags
    );

    // Check that the window was successfully made
    if (window == NULL) {
        // In the event that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    RenderEngine r{window};
    _shader = Shader::getStandard();

    game->initialize(this);

    debug = new DebugInterface();
    debug->initialize(window, game);

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;

    while (!input.quit){

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        delta_time = clamp(((NOW - LAST) / (float)SDL_GetPerformanceFrequency() ),0.0f,1.0f);
        time += delta_time;

        input.update();
        update(delta_time);
        render();

        SDL_Delay(16.0f);
    }


    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();

    delete debug;

    return 0;
}


void Engine::update(float delta_time){
    game->update(delta_time);
    debug->update();
}

void Engine::render(){

    RenderEngine &r = *RenderEngine::instance;
    int w,h;
    SDL_GetWindowSize(window,&w,&h);

    r.getCamera()->setViewport(0,0,w,h);
    r.getCamera()->setPerspectiveProjection(60,w,h,0.1,100);
    r.clearScreen({1,0,0,1});

    for(int i = 0; i < entities.capacity;i++){
        Entity* e = entities[i];
        if(e != nullptr && e->mesh != nullptr){

            glm::mat4 s = glm::scale(mat4(), e->scale);
            glm::mat4 t = glm::translate(mat4(), e->position);
            glm::mat4 a = mat4_cast(e->rotation);

            r.draw(e->mesh,  t * s * a, _shader);

        }

    }

    debug->render();
    r.swapWindow();
}
