#pragma once

#include "../Game.hpp"
#include "../Engine.hpp"
#include "../renderer/imgui/imgui_impl_sdl_gl3.hpp"
#include "../renderer/Mesh.hpp"
#include "SDL.h"
#include "../Input.hpp"
#include <string>

class DebugInterface{
    public:

        Game* game;
        SDL_Window* window;


        float control_speed = 15;
        bool enabled = true;
        int n;

        //menu
        bool menu = true;
        bool hierarchy = false;
        bool scene_manager = false;
        bool create = false;
        bool hotload_shader = true;

        bool game_debug = false;
        bool entity_debug = false;

        //windows

            //scene manager
            char filename[50];

            //create menu


            int type_selected_index = 0;
            static const int type_num_items = 3;
            const char* type_entities[type_num_items] = {"Cube", "Player", "Light"};


        void initialize(SDL_Window* window, Game* game){
            ImGui_ImplSdlGL3_Init(window);

            this->game = game;
            this->window = window;
        }

        void update(float dt){
            if(Input::get_key_on_down(SDL_SCANCODE_ESCAPE)){ 
                enabled       = !enabled;
                if(!menu && !hierarchy && scene_manager && !create) menu = true;
            }
            if(Input::get_key_on_down(SDL_SCANCODE_F2))     menu          = !menu;
            if(Input::get_key_on_down(SDL_SCANCODE_F3))     hierarchy     = !hierarchy;
            if(Input::get_key_on_down(SDL_SCANCODE_F4))     scene_manager = !scene_manager;
            if(Input::get_key_on_down(SDL_SCANCODE_F5))     create        = !create;
            if(Input::get_key_on_down(SDL_SCANCODE_F6))     hotload_shader= !hotload_shader;
            if(Input::get_key_on_down(SDL_SCANCODE_F8))     game_debug    = !game_debug;
            if(Input::get_key_on_down(SDL_SCANCODE_F9))     entity_debug  = !entity_debug;

            //debug camera
        }

        void render(float dt){

            if(enabled){

                ImGui_ImplSdlGL3_NewFrame(window);

                //menu
                if(menu){
                    ImGui::Begin("Menu");
                        ImGui::Checkbox("Debug         [Esc]", &enabled);
                        ImGui::Checkbox("Menu          [F2] ", &menu);
                        ImGui::Checkbox("Hierarchy     [F3] ", &hierarchy);
                        ImGui::Checkbox("Scene Manager [F4] ", &scene_manager);
                        ImGui::Checkbox("Create Menu   [F5] ", &create);

                        ImGui::Checkbox("Hotload Shader[F6] ", &hotload_shader);

                        ImGui::Checkbox("Game Debug    [F8] ", &game_debug);
                        ImGui::Checkbox("Entity Debug  [F9] ", &entity_debug);
                    ImGui::End();
                }

                //game
                if(game_debug)
                    game->draw_debug();

                //windows
                if(hierarchy){
                    ImGui::Begin("Hierarchy");
                    ImGui::DragFloat("Control Speed", &control_speed);
                    for(int i = 0; i < Engine::entities.capacity;i++){
                        Entity* e = Engine::entities[i];
                        if(e != nullptr){
                            ImGui::PushID(i);
                            e->draw_debug_inspector(dt, control_speed);
                            ImGui::PopID();
                        }
                    }
                    ImGui::End();
                }
                
                if(scene_manager){
                    ImGui::Begin("Scene Manager");
                    ImGui::InputText("Filename", filename, 50);
                    if(ImGui::Button("Save")){
                        SceneLoader::save_scene(filename);
                    }

                    ImGui::SameLine();

                    if(ImGui::Button("Load")){
                        SceneLoader::load_scene(filename);
                    }


                    ImGui::End();
                }
                if(create){
                    ImGui::Begin("Create Menu");
                    ImGui::Combo("Type", &type_selected_index, type_entities, type_num_items);
                    if(ImGui::Button("Create")){
                        _create_entity(type_selected_index);
                    }


                    ImGui::End();
                }


                if(entity_debug){
                    for(int i = 0; i < Engine::entities.capacity;i++){
                        Entity* e = Engine::entities[i];
                        if(e != nullptr){
                            e->draw_debug();
                        }
                    }
                }

                ImGui::Render();
            }
        }


        void _create_entity(int index){
            Entity* e = new (Engine::entities.create()) Entity();

            e->name = type_entities[type_selected_index];
            e->mesh = Mesh::get_cube();
            e->position = glm::vec3();
            e->scale = glm::vec3(1,1,1);
            e->rotation = glm::quat();
        }

};
