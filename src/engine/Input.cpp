#include "Input.hpp"

bool Input::keys[284];

void Input::update(){
    SDL_Event e; //An SDL event to figure out which keys are being manipulated
    while (SDL_PollEvent(&e) != 0) { //If there is an event
        ImGui_RE_ProcessEvent(&e);

        switch (e.type){

            case SDL_KEYDOWN:
                if (e.key.repeat == 0) { //if this key is not being held down
                    Input::keys[e.key.keysym.scancode] = true;
                }
            break;

            case SDL_KEYUP:
                Input::keys[e.key.keysym.scancode] = false;
            break;

            case SDL_QUIT:
                quit = true;
            break;
        }
    }
}
