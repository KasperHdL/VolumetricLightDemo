#pragma once 
#include "glm/glm.hpp"
#include "imgui/imgui_impl_sdl_gl3.hpp"
#include <string>

#include "Mesh.hpp"

#include "../Input.hpp"

class Light{
private:
    bool _debug_control = false;
    int type_selected_index;
    static const int type_name_length = 3;
    const char* type_names[type_name_length] = {"Directional Light", "Point Light", "Spot Light"};

public:
    enum class Type{
        Directional,
        Point,
        Spot
    };

    Type type;
    glm::vec3 position;
    glm::vec3 color;

    //[point / spot]
    glm::vec3 attenuation;
    float intensity;
    Mesh* mesh = nullptr;
    glm::vec3 scale = glm::vec3(1,1,1);

    //[spot]
    float falloff;
    glm::vec3 direction;

    //shadow map projection settings
    bool create_shadow_map = false;
    int shadow_map_index = -1;
    glm::mat4 shadow_vp;
    
    //[proj]
    float field_of_view    = 90;

    //[proj and ortho]
    float near_plane       = .1f;
    float far_plane        = 100;

    //[ortho]
    float left_plane       = -10;
    float right_plane      = 10;
    float bottom_plane     = -10;
    float top_plane        = 10;

    

    Light(){
        this->type = Type::Point;
        type_selected_index = (int)type;
        this->position = vec3(0);
        this->color = vec3(1);
        this->intensity = 1;
        attenuation = glm::vec3(1, 0.1f, 0.01f);

        calc_influence_mesh();
    }
    Light(Type type, glm::vec3 position, glm::vec3 color, float intensity){
        //[Point or Directional]
        this->type = type;
        type_selected_index = (int)type;

        this->position  = position;
        this->direction = glm::vec3(0,0,1);
        this->color     = color;
        this->intensity = intensity;

        attenuation = glm::vec3(1, 0.1f, 0.01f);
        falloff = 10;


        calc_influence_mesh();
    } 

    Light(Type type, glm::vec3 position, glm::vec3 direction, float falloff, glm::vec3 color, float intensity){
        //[Spot]
        this->type = type;
        type_selected_index = (int)type;

        this->position = position;
        this->direction = direction;
        this->color = color;
        this->intensity = intensity;
        this->falloff = falloff;

        attenuation = glm::vec3(1, 0.1f, 0.01f);

        calc_influence_mesh();
    }

    void calc_influence_mesh(){
        if(type == Type::Directional){
            mesh = nullptr;
        }else if(type <= Type::Spot){
            //Point or Spot

            mesh = Mesh::get_sphere();

            //super rough approx
            const float min_darkness= (256.0 / 5.0);
            float lightMax  = glm::max(glm::max(color.r, color.g), color.b) * intensity;
            float radius    = (-attenuation.x +  glm::sqrt(attenuation.x * attenuation.x - 4 * attenuation.z * (attenuation.y - min_darkness * lightMax))) / (2 * attenuation.z);  
              
            scale = vec3(radius, radius, radius) * 2.0f;

        }
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

            if(type == Type::Spot){
                ImGui::DragFloat("Fall Off"      , &falloff        , 0.01f);
            }

            if(type == Type::Directional || type == Type::Spot)
                ImGui::Checkbox("Shadow Map", &create_shadow_map);

            if(create_shadow_map){
                if(type == Type::Spot){
                    ImGui::DragFloat("Field of View", &field_of_view, 0.1f);
                    field_of_view = glm::clamp<float>(field_of_view, 1, 360);
                }

                if(type == Type::Directional){
                    ImGui::DragFloat("Left Plane"   , &left_plane   , 0.1f);
                    ImGui::DragFloat("Right Plane"  , &right_plane  , 0.1f);
                    ImGui::DragFloat("Bottom Plane" , &bottom_plane , 0.1f);
                    ImGui::DragFloat("Top Plane"    , &top_plane    , 0.1f);

                }

                ImGui::DragFloat("Near Plane" , &near_plane , 0.1f);
                ImGui::DragFloat("Far Plane"  , &far_plane  , 0.1f);

            }
                ImGui::DragFloat3("Scale"  , &scale.x, 0.1f);

            calc_influence_mesh();

            ImGui::TreePop();

        }
    }

    void set_ortho_scale(float scale){
        near_plane       = -scale;
        left_plane       = -scale;
        bottom_plane     = -scale;
        far_plane        = scale;
        right_plane      = scale;
        top_plane        = scale;
    }


};
