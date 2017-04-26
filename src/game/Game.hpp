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


            SceneLoader::load_scene("test.scene");
            cout << "Loaded \n";
            SceneLoader::save_scene("test1.scene");
            cout << "Saved \n";



        }

        void update(float delta_time){

        }

};
