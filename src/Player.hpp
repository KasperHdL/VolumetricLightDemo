#pragma once 

#include "renderer/Renderer.hpp"

#include "Entity.hpp"
#include "God.hpp" 

#include "renderer/Camera.hpp"
 
#include <string> 
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Player{ 
    public: 
        Entity* entity; 
        Camera* camera;
        DebugInterface* debug;

        float speed = 15;
        float rotation_speed = .3f;
        float rot_max_x = (((float)60/180) * glm::pi<float>());

        bool freeze = false;
 
        Player(){ 
            entity = new (God::entities.create()) Entity();
            entity->name = "Player";
            entity->position = vec3(10,10,10);

            camera = Renderer::instance->camera;
            debug = Renderer::instance->debug;

        } 

        void update(float dt){
            if(Input::get_key_on_down(SDL_SCANCODE_F))
                freeze = !freeze;
            if(freeze){
                return;
            }


            //position
            vec3 move_direction = vec3(0,0,0);

            if(Input::get_key_down(SDL_SCANCODE_W))
                move_direction += vec3(0,0,-1);
            if(Input::get_key_down(SDL_SCANCODE_A))
                move_direction += vec3(-1,0,0);
            if(Input::get_key_down(SDL_SCANCODE_S))
                move_direction += vec3(0,0,1);
            if(Input::get_key_down(SDL_SCANCODE_D))
                move_direction += vec3(1,0,0);
            if(Input::get_key_down(SDL_SCANCODE_SPACE))
                move_direction += vec3(0,1,0);
            if(Input::get_key_down(SDL_SCANCODE_LSHIFT))
                move_direction += vec3(0,-1,0);

            if(move_direction != vec3(0,0,0))
                entity->position += mat3(eulerAngleY(-entity->rotation.y)) * (normalize(move_direction) * speed * dt);

            //rotation
            vec2 delta_mouse = Input::get_mouse_delta();

            entity->rotation.x += delta_mouse.y * rotation_speed * dt;
            entity->rotation.x = clamp<float>(entity->rotation.x, -rot_max_x, rot_max_x);
            entity->rotation.y += delta_mouse.x * rotation_speed * dt;

            //update camera
            camera->view_transform = (eulerAngleX(entity->rotation.x) * eulerAngleY(entity->rotation.y)) * glm::translate(mat4(), -entity->position);
            camera->entity->position = entity->position;
            camera->entity->rotation = entity->rotation;



        }
 
        ~Player(){ 
 
        } 
 
}; 
