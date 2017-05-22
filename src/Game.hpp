#pragma once

#include <string>
#include "Engine.hpp"
#include "utils/SceneLoader.hpp"
#include "God.hpp"
#include <vector>
#include "Player.hpp"


class Game{
    public:
        Engine* engine;
        Player* player;

        Game(){}
        ~Game(){}

        std::vector<Entity*> ents;

        void initialize(Engine* engine){
            this->engine = engine;
            player = new Player();
            player->entity->position = vec3(-3.3,-1.3,11.3);
            player->entity->rotation = vec3(0.15,6.5,0);

            SceneLoader::load_scene("standard.scene");
            God::entities[God::entities.count-1]->specularity = 0;

            for(int x = -10; x < 10; x ++){
                for(int y = -10; y < 10; y ++){
                    Entity* e   = new (God::entities.create()) Entity();
                    e->position = vec3(x,-0.2f,y);
                    e->scale    = vec3(.5f);
                    e->rotation = vec3();
                    e->mesh     = Mesh::get_sphere();
                    e->name     = "E (" + to_string(x) + ", " + to_string(y) + ")";
                    e->color = vec4(0.3f,0.5f,0.8f,1);

                    ents.push_back(e);
                }
            }

            Light* sun =  new (God::lights.create()) Light(Light::Type::Directional, vec3(0,-1,.25f), vec3(0,.1f,.5f), .15f);
            sun->set_ortho_scale(15);

            Light* l = new (God::lights.create()) Light(Light::Type::Spot, vec3(0,5,-10.2f),vec3(0,-0.01,.015),0, vec3(1,1,.8f), 1);

            l->create_shadow_map = true;
            l->falloff = 15;
            l->intensity = 1;
            l->attenuation = vec3(1, 0.1f, 0.001f);
            l->calc_influence_mesh();

        }

        float time     = 0;
        float p_scalar = 0.5f;
        float t_scalar = 0.5f;

        void update(float delta_time){
            time += delta_time;

            //move sphere carpet
            for(int i = 0; i < ents.size(); i++){
                Entity* e = ents[i];
                float y = sin(p_scalar * (e->position.z + e->position.x) + t_scalar * time);
                e->position = vec3(e->position.x,y,e->position.z);

            }


            //move light
            float o  = 0;
            float h  = 4;
            float ho = 2;
            float m  = 3;
            float z  = -5;

            vec3 pos = vec3(m * sin(time * 2 + o), h + ho * sin(time + o), z);
        //    God::lights[0]->position = pos;

            //camera 

            player->update(delta_time);



        }

        void draw_debug(){


        }

};
