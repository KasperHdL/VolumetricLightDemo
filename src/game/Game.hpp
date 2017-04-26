#pragma once

#include "../engine/Engine.hpp"
#include "Player.hpp"

class Game{
    public:
        Engine* engine;
        Player player;

        Game(){}
        ~Game(){}

        void initialize(Engine* engine){
            this->engine = engine;
            player = Player();
        }

        void update(float delta_time){

        }

};
