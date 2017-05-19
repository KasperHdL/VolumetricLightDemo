#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <vector>

#include "../renderer/Shader.hpp"
#include "../renderer/Mesh.hpp"
#include "../utils/ObjParser.hpp"
#include "DynamicPool.hpp"
#include "FileLoader.hpp"

class AssetManager{
public: 
    //metadata
    //pools


    static DynamicPool<Shader> shader_pool;
    static vector<Mesh*> mesh_pool;

    static Shader* get_shader(string vertex_shader, string fragment_shader);
    static Shader* get_shader(string shader);

    static Mesh* get_mesh(string name);

    static void update();

    static void cleanup();


private:
    static void _check_shader(Shader* shader);


};
