#include "Input.hpp"

bool Input::_last[284] = {0};
bool Input::_now[284] = {0};

void Input::update(){
    for(int i = 0; i < 284; i++)
        _last[i] = _now[i];

    SDL_Event e; //An SDL event to figure out which keys are being manipulated
    while (SDL_PollEvent(&e) != 0) { //If there is an event

        ImGui_Renderer_ProcessEvent(&e);

        switch (e.type){
            case SDL_KEYDOWN:
                _now[e.key.keysym.scancode] = true;
            break;
            case SDL_KEYUP:
                _now[e.key.keysym.scancode] = false;
            break;
            case SDL_QUIT:
                quit = true;
            break;
        }
    }

}

bool Input::get_key_down(Uint8 key){
    return Input::_now[key];
}

bool Input::get_key_on_down(Uint8 key){
    return (Input::_last[key] ^ Input::_now[key]) & Input::_now[key];
}

