#pragma once

#include "SDL.h"
#include <string>

#include "../God.hpp"
#include "../Game.hpp"
#include "../Input.hpp"

#include "../renderer/imgui/imgui_impl_sdl_gl3.hpp"
#include "../renderer/Mesh.hpp"

class DebugInterface{
    public:

        Game* game;
        SDL_Window* window;


        bool enabled = true;
        int n;

        //plots
        static const int array_length = 500;
        int array_index = 0;
        float delta_time_plot[array_length] = {};
        float delta_time_sum = 0;


        //fog
        float fog_intensity = 0.005;
        vec3 fog_color = vec3(.3f, .4f, .5f);

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
            static const int type_num_items = 5;
            const char* type_entities[type_num_items] = {"Cube", "Quad", "Sphere", "Player", "Light"};

            //hierarchy
            float control_speed = 15;
            bool hierarchy_lights = true;
            bool draw_light_pos = true;


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
                int id = 0;

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

                        ImGui::ColorEdit3("Fog Color", &fog_color.r);
                        ImGui::DragFloat("Fog Intensity", &fog_intensity);
                        //DT Plot
                        
                        delta_time_sum -= delta_time_plot[array_index];
                        delta_time_plot[array_index] = dt;
                        delta_time_sum += delta_time_plot[array_index];


                        ImGui::Separator();

                        ImGui::Text("Delta time in ms");
                        ImGui::PlotLines("", delta_time_plot, array_length);
                        ImGui::Text("%3.3f \t- Avg: %3.3f",1000 * dt, (1000 * delta_time_sum) / (double)array_length);

                        array_index = ++array_index % array_length;

                    ImGui::End();
                }

                //windows
                if(hierarchy){
                    ImGui::Begin("Hierarchy");
                    ImGui::DragFloat("Control Speed", &control_speed);
                    ImGui::Checkbox("Show Lights", &hierarchy_lights);
                    ImGui::Checkbox("Show Light Pos", &draw_light_pos);

                    //light

                    if(hierarchy_lights){
                        for(int i = 0; i < God::lights.capacity;i++){
                            Light* l = God::lights[i];
                            if(l != nullptr){
                                ImGui::PushID(id++);
                                l->draw_debug_inspector(dt, control_speed);
                                ImGui::PopID();
                            }
                        }
                    }

                    //entities
                    for(int i = 0; i < God::entities.capacity;i++){
                        Entity* e = God::entities[i];
                        if(e != nullptr){
                            ImGui::PushID(id++);
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
                    for(int i = 0; i < God::entities.capacity;i++){
                        Entity* e = God::entities[i];
                        if(e != nullptr){
                            e->draw_debug();
                        }
                    }
                }

                ImGui::Render();
            }
        }


        void _create_entity(int index){
            Entity* e = new (God::entities.create()) Entity();

            e->name = type_entities[type_selected_index];
            if(e->name == "Cube")
                e->mesh = Mesh::get_cube();
            else if(e->name == "Quad")
                e->mesh = Mesh::get_quad();
            else if(e->name == "Sphere")
                e->mesh = Mesh::get_sphere();

            e->position = glm::vec3();
            e->scale = glm::vec3(1,1,1);
            e->rotation = glm::vec3();
        }

};
