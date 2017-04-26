#pragma once


#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "render/Mesh.hpp"
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


};
