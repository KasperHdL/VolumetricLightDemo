#pragma once

#include "../God.hpp"
#include "../Entity.hpp"

class Camera{
public:

    Entity* entity;

    mat4 view_transform;
    mat4 projection_transform;

    int viewport_x;
    int viewport_y;
    int viewport_w;
    int viewport_h;

    float field_of_view = 60;
    float near_plane = 1;
    float far_plane = 1000;

    Camera(){
        view_transform = mat4(1);
        projection_transform = mat4(1);

        entity = new (God::entities.create()) Entity(); 
        entity->name = "Camera";
        entity->mesh = nullptr;
        entity->position = vec3();
        entity->scale = vec3();
        entity->rotation = vec3();
        entity->debug_control = false;

    }

    void set_perspective_projection(){
        projection_transform = glm::perspectiveFov<float>(radians(field_of_view), (float)viewport_w, (float)viewport_h, (float)near_plane, (float)far_plane);
    }

    void set_viewport(int x, int y, int w, int h){

        viewport_x = x;
        viewport_y = y;
        viewport_w = w;
        viewport_h = h;

        glViewport(x,y,w,h);
        glScissor(x,y,w,h);

    }

};
