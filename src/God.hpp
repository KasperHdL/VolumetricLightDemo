#pragma once

#include "utils/DynamicPool.hpp"

class Entity;
class Light;
class Camera;

class God{
    public:

    static DynamicPool<Entity> entities;
    static DynamicPool<Light> lights;

};
