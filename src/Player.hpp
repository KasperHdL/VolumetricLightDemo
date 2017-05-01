#pragma once 
 
#include "Engine.hpp" 
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
