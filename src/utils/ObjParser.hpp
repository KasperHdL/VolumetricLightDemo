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
    static bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }


    static Mesh* load(string filename){

        vector<vec3> vertices;
        vector<vec3> normals;

        vector<vec3> tmp_vertices;
        vector<vec3> tmp_normals;

        vector<int> vectices_indices;
        vector<int> normals_indices;

        ifstream file(DataPath::get(filename));
        string line;

        if(file.is_open()){
            while(getline(file, line)){
                if(line.size() != 0  && line.at(0) != '#'){
                    int index = line.find(" ");
                    if(index != -1){
                        string type = line.substr(0,index);
                        line = line.substr(index + 1, line.size() - (index + 1));
                        std::stringstream s(line);

                        if(type == "v"){
                            vec3 v;
                            s >> v.x >> v.y >> v.z;
                            tmp_vertices.push_back(v);
                        }else if(type == "vn"){
                            vec3 v;
                            s >> v.x >> v.y >> v.z;
                            tmp_normals.push_back(v);
                        }else if(type == "f"){
                            string s = line;
                            string tuple;

                            for(int v = 0; v < 3; v++){
                                if(v < 2){
                                    index = s.find(" ");

                                    tuple = s.substr(0, index);
                                    s = s.substr(index+1, s.size() - (index+1));
                                }else{
                                    tuple = s;
                                }

                                //
                                index = tuple.find("//");
                                string value = tuple.substr(0, index);
                                vectices_indices.push_back(atoi(value.c_str()));

                                value = tuple.substr(index+2, tuple.size() - (index+2));
                                normals_indices.push_back(atoi(value.c_str()));

                            }




                        }
                    }
                }
            }

            file.close();

        }else{
            cout << "Cannot open file " << filename << "\n";
            return nullptr;
        }

        //add verts and normals
        for(int i = 0; i < vectices_indices.size(); i++){
            vertices.push_back(tmp_vertices[vectices_indices[i]-1]);
            normals.push_back(tmp_normals[normals_indices[i]-1]);
        }



        vector<vec4> uvs;
        vector<uint16_t> indices;
        vector<vec4> colors;

        return new Mesh(vertices, normals, uvs, colors, indices);
    }




};
