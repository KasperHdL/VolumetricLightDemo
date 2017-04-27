#include "Input.hpp"

const Uint8* Input::_last;
const Uint8* Input::_now;

void Input::update(){
    SDL_Event e; //An SDL event to figure out which keys are being manipulated
    while (SDL_PollEvent(&e) != 0) { //If there is an event
        ImGui_RE_ProcessEvent(&e);

        switch (e.type){
            case SDL_QUIT:
                quit = true;
            break;
        }
    }

    Input::_last = Input::_now;
    Input::_now = SDL_GetKeyboardState(NULL);
}

bool Input::get_key_down(Uint8 key){
    return Input::_now[key];
}

bool Input::get_key_on_down(Uint8 key){
    return (Input::_last[key] | Input::_now[key]) & Input::_now[key];
}

