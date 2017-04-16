#include <iostream>
#include <vector>
#include <fstream>

#include "SRE/Texture.hpp"
#include "SRE/SimpleRenderEngine.hpp"
#include "SRE/Camera.hpp"
#include "SRE/Mesh.hpp"
#include "SRE/Shader.hpp"
#define SDL_MAIN_HANDLED
#include "SDL.h"

#include <glm/glm.hpp>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/glm.hpp>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace SRE;

SDL_Window *window;
Shader *shader;
Mesh *mesh;

int i=0;

void update();

int main() {

    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
            "An SDL2 window",                     // window title
            SDL_WINDOWPOS_UNDEFINED,              // initial x position
            SDL_WINDOWPOS_UNDEFINED,              // initial y position
            640,                                  // width, in pixels
            480,                                  // height, in pixels
            SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE// flags
    );

    // Check that the window was successfully made
    if (window == NULL) {
        // In the event that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SimpleRenderEngine r{window};

    r.getCamera()->lookAt({0,0,3},{0,0,0},{0,1,0});
    shader = Shader::getStandard();
    shader->set("color", glm::vec4(1.0f,1.0f,1.0f,1.0f));
    shader->set("specularity",20.0f);
    mesh = Mesh::create().withCube().build();

    r.setAmbientLight({0.5,0.5,0.5});
    r.setLight(0, Light::create().withPointLight({0, 3,0}).withColor({1,0,0}).withRange(20).build());
    r.setLight(1, Light::create().withPointLight({3, 0,0}).withColor({0,1,0}).withRange(20).build());
    r.setLight(2, Light::create().withPointLight({0,-3,0}).withColor({0,0,1}).withRange(20).build());
    r.setLight(3, Light::create().withPointLight({-3,0,0}).withColor({1,1,1}).withRange(20).build());

    bool quit=false;
    while (!quit){
        SDL_Event e;
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            if (e.type == SDL_QUIT)
                quit = true;
        }
        update();
        SDL_Delay(16);
    }


    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();

    return 0;
}

void update() {
    SimpleRenderEngine &r = *SimpleRenderEngine::instance;
    int w,h;
    SDL_GetWindowSize(window,&w,&h);
    r.getCamera()->setViewport(0,0,w,h);
    r.getCamera()->setPerspectiveProjection(60,w,h,0.1,100);
    r.clearScreen({1,0,0,1});
    r.draw(mesh, glm::eulerAngleY(glm::radians((float)i)), shader);
    r.swapWindow();
    i++;
}

