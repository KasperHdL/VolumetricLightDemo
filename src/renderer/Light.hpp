#pragma once 
#include "glm/glm.hpp"

class Light{
public:
    enum class Type{
        Directional,
        Point,
        Spot,
        Volumetric
    };

    glm::vec3 position;

    glm::vec3 color;
    float intensity;

    Type type;

    Light(Type type, glm::vec3 position, glm::vec3 color, float intensity){
        this->type = type;

        this->position = position;
        this->color = color;
        this->intensity = intensity;

    } 

};
