#pragma once


#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "render/Mesh.hpp"
#include "render/imgui/imgui_re.hpp"
#include <string>

using namespace glm;

class Entity
{
public:
    std::string name;

    //Transform
    vec3 position;
    vec3 scale;
    quat rotation;

    //Render Properties
    
    Mesh* mesh;


    Entity(){
    }

    ~Entity(){

    }

    void draw_debug(){

    }

    void draw_debug_inspector(){
        if(ImGui::TreeNode(name.c_str())){

            ImGui::DragFloat3("Position", &position.x,0.1f);
            ImGui::DragFloat3("Scale", &scale.x,0.1f);
            ImGui::DragFloat4("Rotation", &rotation.x,0.1f);

            ImGui::TreePop();
        }
    }


};
