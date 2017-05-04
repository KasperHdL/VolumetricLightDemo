#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

#include "../renderer/Shader.hpp"
#include "../renderer/Mesh.hpp"
#include "DynamicPool.hpp"
#include "FileLoader.hpp"

class AssetManager{
public: 
    //metadata
    //pools


    static DynamicPool<Shader> shader_pool;

    static Shader* get_shader(string vertex_shader, string fragment_shader);
    static Shader* get_shader(string shader);

    static void update();


private:
    static void _check_shader(Shader* shader);


};
