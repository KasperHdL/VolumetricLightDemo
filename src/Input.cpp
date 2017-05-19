#include "Input.hpp"

bool Input::_last[284] = {0};
bool Input::_now[284] = {0};

glm::vec2 Input::_last_mouse = glm::vec2();
glm::vec2 Input::_current_mouse = glm::vec2();

void Input::update(){
    for(int i = 0; i < 284; i++)
        _last[i] = _now[i];

    _last_mouse = _current_mouse;

    SDL_Event e; //An SDL event to figure out which keys are being manipulated
    while (SDL_PollEvent(&e) != 0) { //If there is an event

        ImGui_ImplSdlGL3_ProcessEvent(&e);

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
            case SDL_MOUSEMOTION:
                _current_mouse.x = e.motion.x;
                _current_mouse.y = e.motion.y;
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

glm::vec2 Input::get_mouse_delta(){
    return _current_mouse - _last_mouse;
}

glm::vec2 Input::get_mouse_position(){
    return _current_mouse;
}

