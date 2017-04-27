#pragma once

#include "../../game/Game.hpp"
#include "../Engine.hpp"
#include "../render/imgui/imgui_re.hpp"
#include "SDL.h"
#include "../Input.hpp"

class DebugInterface{
    public:

        Game* game;
        SDL_Window* window;

        bool enabled;
        int n;

        //menu
        bool hierarchy;
        bool save_load;

        void initialize(SDL_Window* window, Game* game){
            hierarchy = false;
            save_load = false;

            ImGui_RE_Init(window);

            this->game = game;
            this->window = window;
        }

        void update(){
            if(Input::get_key_on_down(SDL_SCANCODE_ESCAPE))
                enabled = !enabled;
        }

        void render(){

            if(enabled){

                ImGui_RE_NewFrame(window);

                //menu


                //game
                game->draw_debug();

                //windows
                ImGui::Begin("Hierarchy");
                for(int i = 0; i < Engine::entities.capacity;i++){
                    Entity* e = Engine::entities[i];
                    if(e != nullptr){
                        e->draw_debug_inspector();
                    }
                }
                ImGui::End();

                ImGui::Begin("Input");

                const Uint8* keys = SDL_GetKeyboardState(NULL);
                for(int i = 0; i < 284; i++){
                    ImGui::Text((keys[i] ? "X" : " "));


                }
                ImGui::End();

                for(int i = 0; i < Engine::entities.capacity;i++){
                    Entity* e = Engine::entities[i];
                    if(e != nullptr){
                        e->draw_debug();
                    }
                }

                ImGui::Render();
            }
        }

};
