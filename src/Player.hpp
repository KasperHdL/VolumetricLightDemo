#pragma once 

#include "Entity.hpp"
#include "God.hpp" 
#include <string> 
 
class Player{ 
    public: 
        Entity* entity; 
 
        Player(){ 
            entity = new (God::entities.create()) Entity();
            entity->name = "Player";
 
        } 
 
        ~Player(){ 
 
        } 
 
}; 
