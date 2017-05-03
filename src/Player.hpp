#pragma once 

#include "Entity.hpp"
#include "Engine.hpp" 
#include <string> 
 
class Player{ 
    public: 
        Entity* entity; 
 
        Player(){ 
            entity = new (Engine::entities.create()) Entity();
            entity->name = "Player";
 
        } 
 
        ~Player(){ 
 
        } 
 
}; 
