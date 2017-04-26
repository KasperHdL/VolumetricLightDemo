#pragma once

#include "../engine/Engine.hpp"

class Player{
    public:
        Entity* entity;

        Player(){
            entity = Engine::entities.create();

        }

        ~Player(){

        }

};

