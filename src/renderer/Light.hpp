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

    glm::vec4 position;

    glm::vec3 color;
    float intensity;

    Type type;

    Light(Type type, glm::vec3 vector, glm::vec3 color, float intensity){
        this->type = type;

        this->position = glm::vec4(vector, type);
        this->color = color;
        this->intensity = intensity;

    } 

};
