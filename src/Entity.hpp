#pragma once


#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "SDL.h"
#include "Input.hpp"
#include "renderer/imgui/imgui_impl_sdl_gl3.hpp"
#include "renderer/Mesh.hpp"
#include <string>

using namespace glm;

class Entity
{
public:
    bool debug_control = false;
    std::string name;

    Mesh* mesh = nullptr;

    //Transform
    vec3 position;
    vec3 scale;
    vec3 rotation;

    vec4 color = vec4(1,1,1,1);
    float specularity = 50;

    //Render Properties

    Entity(){
    }

    ~Entity(){

    }

    void draw_debug_inspector(float dt, float control_speed){
        if(ImGui::TreeNode(name.c_str())){
            ImGui::Checkbox("Control", &debug_control);
            if(debug_control){
                if(Input::get_key_down(SDL_SCANCODE_W))
                    position.z += control_speed * dt;
                 if(Input::get_key_down(SDL_SCANCODE_A))
                    position.x += control_speed * dt;
                if(Input::get_key_down(SDL_SCANCODE_S))
                    position.z -= control_speed * dt;
                if(Input::get_key_down(SDL_SCANCODE_D))
                    position.x -= control_speed * dt;
                if(Input::get_key_down(SDL_SCANCODE_SPACE))
                    position.y += control_speed * dt;
                if(Input::get_key_down(SDL_SCANCODE_LSHIFT))
                    position.y -= control_speed * dt;
            }

            ImGui::DragFloat3("Position", &position.x, 0.1f);
            ImGui::DragFloat3("Scale",    &scale.x,    0.1f);
            ImGui::DragFloat3("Rotation", &rotation.x, 0.01f);
            ImGui::ColorEdit3("Color", &color.x);
            ImGui::DragFloat("Specularity", &specularity);

            ImGui::TreePop();
        }
    }


};
