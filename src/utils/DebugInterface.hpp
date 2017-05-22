#pragma once

#include "SDL.h"
#include <string>

#include "../God.hpp"
#include "../Game.hpp"
#include "../Input.hpp"

#include "../renderer/imgui/imgui_impl_sdl_gl3.hpp"
#include "../renderer/Mesh.hpp"
#include "../renderer/Light.hpp"

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
        float fog_intensity = 0.004;
        vec3 fog_color = vec3(.3f, .4f, .3f);
        float position_rand = 2;

        vec3 ray_att = vec3(0.2f, 0.01f, 0.001f);
        float ray_rand = 1;
        float light_color_rand = 0;

        float kernel[9] = {0,0,0,0,1,0,0,0,0};

        //menu
        bool menu = true;
        bool hierarchy = false;
        bool scene_manager = false;
        bool create = false;
        bool hotload_shader = true;

        //windows

            //scene manager
            char filename[50];

            //create menu
            int type_selected_index = 0;
            static const int type_num_items = 4;
            const char* type_entities[type_num_items] = {"Cube", "Quad", "Sphere", "Light"};

            //hierarchy
            float control_speed = 15;
            bool draw_light_pos = false;


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

        }

        void render(float dt){

            if(enabled){
                int id = 0;

                ImGui_ImplSdlGL3_NewFrame(window);

                //menu
                if(menu){
                    ImGui::Begin("Menu");
                        ImGui::Text("Move on [WASD], [Space] and [Shift]");
                        ImGui::Checkbox("Freeze Camera [F]", &game->player->freeze);
                        ImGui::Separator();

                        ImGui::Checkbox("Debug         [Esc]", &enabled);
                        ImGui::Checkbox("Menu          [F2] ", &menu);
                        ImGui::Checkbox("Hierarchy     [F3] ", &hierarchy);
                        ImGui::Checkbox("Scene Manager [F4] ", &scene_manager);
                        ImGui::Checkbox("Create Menu   [F5] ", &create);

                        ImGui::Checkbox("Hotload Shader[F6] ", &hotload_shader);

                        ImGui::Separator();

                        ImGui::Text("Fog");
                        ImGui::ColorEdit3("Color", &fog_color.r);
                        ImGui::DragFloat("Intensity", &fog_intensity,0.001f);
                        ImGui::DragFloat("Position Rand Scalar", &position_rand);

                        ImGui::Separator();

                        ImGui::Text("Raymarching");
                        ImGui::DragFloat3("Attenuation", &ray_att.x, 0.01f);
                        ImGui::DragFloat("Rand scalar", &ray_rand, 0.1f);

                        if(ray_att.x <= 0) ray_att.x = 0.2f;
                        if(ray_att.y <= 0) ray_att.y = 0.01f;
                        if(ray_att.z <= 0) ray_att.z = 0.001f;

                        if(glm::length(ray_att) == 0) ray_att.x = 0.1f;

                        ImGui::Separator();

                        ImGui::Text("Light Color Random Offset");
                        ImGui::DragFloat("Light Color Rand", &light_color_rand, 0.01f);

                        ImGui::Separator();
                        ImGui::Text("Kernel");
                        ImGui::DragFloat3("[1]", &kernel[0]);
                        ImGui::DragFloat3("[2]", &kernel[3]);
                        ImGui::DragFloat3("[3]", &kernel[6]);
                        ImGui::Separator();

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
                    ImGui::Checkbox("Show Light Pos", &draw_light_pos);
                    ImGui::Separator();

                    //light

                    ImGui::Text("");
                    ImGui::Text("Lights:");
                    ImGui::Separator();
                    for(int i = 0; i < God::lights.capacity;i++){
                        Light* l = God::lights[i];
                        if(l != nullptr){
                            ImGui::PushID(id++);
                            l->draw_debug_inspector(dt, control_speed);
                            ImGui::PopID();
                            ImGui::Separator();
                        }
                    }

                    ImGui::Text("");
                    ImGui::Text("Entities:");
                    ImGui::Separator();
                    //entities
                    for(int i = 0; i < God::entities.capacity;i++){
                        Entity* e = God::entities[i];
                        if(e != nullptr){
                            ImGui::PushID(id++);
                            e->draw_debug_inspector(dt, control_speed);
                            ImGui::PopID();
                            ImGui::Separator();
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

                ImGui::Render();
            }
        }


        void _create_entity(int index){
            if(index == 3){
                //Light
                Light* l = new (God::lights.create()) Light();

            }else{
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
        }

};
