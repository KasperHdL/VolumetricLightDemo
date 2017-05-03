#include "Engine.hpp"

#include "Game.hpp"
#include "utils/DebugInterface.hpp"


DynamicPool<Entity> Engine::entities = DynamicPool<Entity>(32);
//Camera* Engine::camera;

Engine::Engine(int screen_width, int screen_height){
    this->screen_width = screen_width;
    this->screen_height = screen_height;
}

Engine::~Engine(){
}

int Engine::initialize(Game* game){
    this->game = game;

    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

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

    Renderer renderer;
    renderer.initialize(window, screen_width, screen_height);
 //   Engine::camera = r.getCamera();

    //update_shaders();

    game->initialize(this);

    debug = new DebugInterface();
    debug->initialize(window, game);
    renderer.debug = debug;

    //r.setAmbientLight({0.5,0.5,0.5});
    //r.setLight(0, Light::create().withDirectionalLight(glm::vec3(.1f,.7f,.1f)).withColor(glm::vec3(1.0f,1.0f,1.0f)).build());

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;

    while (!input.quit){

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        delta_time = clamp(((NOW - LAST) / (float)SDL_GetPerformanceFrequency() ),0.0f,1.0f);
        time += delta_time;

        update(delta_time);
        renderer.render();

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
    input.update();
    //Engine::camera->update();
    game->update(delta_time);
    debug->update(delta_time);
    if(debug->hotload_shader){
        //update_shaders();
    }
}

