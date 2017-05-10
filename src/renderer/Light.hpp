#pragma once 
#include "glm/glm.hpp"
#include "imgui/imgui_impl_sdl_gl3.hpp"
#include "../Input.hpp"
#include <string>

class Light{
private:
    bool _debug_control = false;
    int type_selected_index;
    static const int type_name_length = 4;
    const char* type_names[type_name_length] = {"Directional Light", "Point Light", "Spot Light", "Volumetric Light"};

public:
    enum class Type{
        Directional,
        Point,
        Spot,
        Volumetric
    };

    glm::vec3 position;
    glm::vec3 direction;
    float cutoff;

    glm::vec3 color;
    glm::vec3 attenuation;
    float intensity;

    Type type;

    Light(Type type, glm::vec3 position, glm::vec3 color, float intensity){
        //Point or Directional
        this->type = type;
        type_selected_index = (int)type;

        this->position = position;
        this->direction = glm::vec3(0,0,1);
        this->color = color;
        this->intensity = intensity;

        attenuation = glm::vec3(1, 0.1f, 0.01f);
        cutoff = 1.0f;

    } 

    Light(Type type, glm::vec3 position, glm::vec3 direction, float cutoff, glm::vec3 color, float intensity){
        //Spot
        this->type = type;
        type_selected_index = (int)type;

        this->position = position;
        this->direction = direction;
        this->color = color;
        this->intensity = intensity;
        this->cutoff = cutoff;

        attenuation = glm::vec3(1, 0.1f, 0.01f);

    }

    void draw_debug_inspector(float dt, float control_speed){
        if(ImGui::TreeNode(type_names[type_selected_index])){
            ImGui::Checkbox("Control", &_debug_control);

            if(_debug_control){
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

            ImGui::Combo("Type", &type_selected_index, type_names, type_name_length);
            if((Type)type_selected_index != type){
                type = (Type)type_selected_index;
            }

            if(type >= Type::Point)
                ImGui::DragFloat3("Position"    , &position.x    , 0.1f);

            if(type == Type::Directional)
                ImGui::DragFloat3("Direction"   , &position.x    , 0.1f);

            if(type == Type::Spot)
                ImGui::DragFloat3("Direction"   , &direction.x   , 0.1f);

            ImGui::ColorEdit3("Color"           , &color.r);
            ImGui::DragFloat("Intensity"        , &intensity     , 0.1f);

            if(type >= Type::Point)
                ImGui::DragFloat3("Attenuation" , &attenuation.x , 0.01f);

            if(type == Type::Spot)
                ImGui::DragFloat("Cut Off"      , &cutoff        , 0.01f);

            ImGui::TreePop();

        }
    }


};
