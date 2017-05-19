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

            SceneLoader::load_scene("standard.scene");

            Entity* e   = new (God::entities.create()) Entity();
            e->position = vec3(0,0,0);
            e->scale    = vec3(1,1,1);
            e->rotation = vec3(0,0,0);
            e->mesh     = AssetManager::get_mesh("landscape.obj");
            e->name     = "Landscape";

            /*
            for(int x = -10; x < 10; x ++){
                for(int y = -10; y < 10; y ++){
                    Entity* e   = new (God::entities.create()) Entity();
                    e->position = vec3(x,0,y);
                    e->scale    = vec3(.5f);
                    e->rotation = vec3();
                    e->mesh     = Mesh::get_sphere();
                    e->name     = "E (" + to_string(x) + ", " + to_string(y) + ")";

                    ents.push_back(e);
                }
            }*/


//            new (God::lights.create()) Light(Light::Type::Directional, vec3(-1,-1,1), vec3(0,1,0), 1);
            new (God::lights.create()) Light(Light::Type::Spot, vec3(0,5,-10.2f),vec3(0,-0.01,1),0, vec3(1,1,1), 1);

            God::lights[0]->create_shadow_map = true;
            God::lights[0]->falloff = 15;
            God::lights[0]->intensity = 4;
            God::lights[0]->calc_influence_mesh();
            //God::lights[0]->far_plane = 10;

            //new (God::lights.create()) Light(Light::Type::Spot, vec3(0,0,-1), vec3(0,0,1), 1);


        }

        float time     = 0;
        float p_scalar = 0.5f;
        float t_scalar = 0.5f;
        void update(float delta_time){
            time += delta_time;

            for(int i = 0; i < ents.size(); i++){
                Entity* e = ents[i];
                float y = sin(p_scalar * (e->position.z + e->position.x) + t_scalar * time);
                e->position = vec3(e->position.x,y,e->position.z);

            }

            float o  = 0;
            float h  = 4;
            float ho = 2;
            float m  = 3;
            float z  = -5;

            vec3 pos = vec3(m * sin(time * 2 + o), h + ho * sin(time + o), z);
            God::lights[0]->position = pos;

            /*
            o = 2;
            pos = vec3(m * sin(time + o)     , h + ho * sin(time + o), z);
            God::lights[1]->position = pos;

            o = 4;
            pos = vec3(m * sin(time + o)     , h + ho * sin(time + o), z);
            God::lights[2]->position = pos;

            */

            //camera 

            player->update(delta_time);



        }

        void draw_debug(){


        }

};
