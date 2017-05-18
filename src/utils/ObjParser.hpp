#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../renderer/Mesh.hpp"

                            

#include <string>
#include <sstream>
#include "DataPath.hpp"


class ObjParser{
public:

    static Mesh* load(string filename){

        vector<vec3> vertices;
        vector<vec3> normals;
        vector<vec4> uvs;
        vector<uint16_t> indices;

        vector<vec3> tmp_vertices;
        vector<vec3> tmp_normals;
        vector<vec2> tmp_uvs;

        ifstream file(DataPath::get(filename));
        string line;

        if(file.is_open()){
            while(getline(file, line)){
                if(line.size() != 0  && line.at(0) != '#'){
                    int index = line.find(" ");
                    if(index != -1){
                        string type = line.substr(0,index);
                        line = line.substr(index, line.size() - index);
                        std::stringstream s(line);

                        if(type == "v"){
                            vec3 v;
                            s >> v.x >> v.y >> v.z;
                            tmp_vertices.push_back(v);
                        }else if(type == "vt"){
                            vec3 v;
                            s >> v.x >> v.y >> v.z;
                            tmp_normals.push_back(v);
                        }else if(type == "vn"){
                            vec2 v;
                            s >> v.x >> v.y;
                            tmp_uvs.push_back(v);
                        }else if(type == "f"){





                        }
                    }
                }
            }

            file.close();

        }else{
            cout << "Cannot open file " << filename << "\n";
        }
        vector<vec4> colors;
        return new Mesh(vertices, normals, uvs, colors, indices);
    }




};
