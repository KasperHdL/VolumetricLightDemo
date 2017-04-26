#pragma once

#include "../engine/Engine.hpp"
#include <string>

class Player{
    public:
        Entity* entity;

        Player(){
            entity = Engine::entities.create();
            entity->name = "Player";

        }

        ~Player(){

        }

};

