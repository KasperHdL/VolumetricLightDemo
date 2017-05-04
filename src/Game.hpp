#pragma once

#include <string>
#include "Engine.hpp"
#include "utils/SceneLoader.hpp"
#include "God.hpp"
#include <vector>


class Game{
    public:
        Engine* engine;

        Game(){}
        ~Game(){}

        std::vector<Entity*> ents;

        void initialize(Engine* engine){
            this->engine = engine;

            SceneLoader::load_scene("standard.scene");

            for(int x = -10; x < 10; x ++){
                for(int y = -10; y < 10; y ++){
                    Entity* e = new (God::entities.create()) Entity();
                    e->position = vec3(x,0,y);
                    e->scale = vec3(.5f);
                    e->rotation = quat();
                    e->mesh = Mesh::get_sphere();
                    e->name = "E (" + to_string(x) + ", " + to_string(y) + ")";

                    ents.push_back(e);
                }
            }

        }

        float time = 0;
        float p_scalar = 0.5f;
        float t_scalar = 0.5f;
        void update(float delta_time){
            time += delta_time;

            for(int i = 0; i < ents.size(); i++){
                Entity* e = ents[i];
                float y = sin(p_scalar * (e->position.z + e->position.x) + t_scalar * time);
                e->position = vec3(e->position.x,y,e->position.z);

            }


        }

        void draw_debug(){


        }

};
