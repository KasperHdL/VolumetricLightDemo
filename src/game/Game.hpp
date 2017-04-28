#pragma once

#include <string>
#include "../engine/Engine.hpp"
#include "Player.hpp"
#include "../engine/utils/SceneLoader.hpp"

class Game{
    public:
        Engine* engine;
        Player player;

        Game(){}
        ~Game(){}

        void initialize(Engine* engine){
            this->engine = engine;

            SceneLoader::load_scene("standard.scene");

        }

        void update(float delta_time){

        }

        void draw_debug(){


        }

};
