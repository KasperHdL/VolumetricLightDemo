#pragma once

#include "impl/GL.hpp"
#include "glm/glm.hpp"
#include <vector>
#include <string>

using namespace std;
using namespace glm;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

class Mesh{
public:
    string name = "";


    enum class Topology{
        points          = 0x0000,
        lines           = 0x0001,
        lines_strip     = 0x0003,
        triangles       = 0x0004,
        triangles_strip = 0x0005,
        triangle_fan    = 0x0006
    };

    unsigned int vertex_buffer_id;
    unsigned int element_buffer_id;
    unsigned int vertex_array_object;
    int vertex_count;

    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec4> uvs;
    vector<vec4> colors;
    vector<uint16_t> indices;

    Topology topology;

    Mesh(vector<vec3>& positions, vector<vec3>& normals, vector<vec4>& uvs, vector<vec4>& colors, vector<uint16_t>& indices){
        glGenBuffers(1, &vertex_buffer_id);
        glGenBuffers(1, &element_buffer_id);
        glGenVertexArrays(1, &vertex_array_object);

        update(positions, normals, uvs, colors, indices);
    }

    ~Mesh(){
        glDeleteVertexArrays(1, &vertex_array_object);
        glDeleteBuffers(1, &vertex_buffer_id);
        glDeleteBuffers(1, &element_buffer_id);
    }

    void update(vector<vec3>& positions, vector<vec3>& normals, vector<vec4>& uvs, vector<vec4>& colors, vector<uint16_t>& indices){
        this->positions = positions;
        this->normals = normals;
        this->uvs = uvs;
        this->colors = colors;
        this->indices = indices;

        this->vertex_count = (int)positions.size();

        topology = Topology::triangles;

        bool hasNormals = normals.size() == vertex_count;
        bool hasUVs     = uvs.size()     == vertex_count;
        bool hasColors  = colors.size()  == vertex_count;


        //interleave data

        const int floats_per_vertex = 14;
        vector<float> interleaved_data(positions.size() * floats_per_vertex);

        for(int i = 0; i < vertex_count; i++){
            for(int j = 0; j < 4; j++){
                int index = i * floats_per_vertex + j;
                if(j < 3){
                    interleaved_data[index] = positions[i][j];
                    interleaved_data[index + 3] = hasNormals ? normals[i][j] : 0.0f;
                }

                interleaved_data[index + 6] = hasUVs ? uvs[i][j] : j==2 ? 1.0f : 0.0f;
                interleaved_data[index + 10] = hasColors ? colors[i][j] : 1.0f;
            }
        }

        glBindVertexArray(vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*interleaved_data.size(), interleaved_data.data(), GL_STATIC_DRAW);


        //set vertex attribute pointers
        set_vertex_attribute_pointers();

        //set indices if non empty
        if(!indices.empty()){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
            GLsizeiptr indices_size = indices.size() * sizeof(uint16_t);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices.data(), GL_STATIC_DRAW);
        }

    }

    void bind(){
//        glBindVertexArray(vertex_array_object);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
        set_vertex_attribute_pointers();

        if(indices.empty())
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        else
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
    }

    void set_vertex_attribute_pointers(){
        const int floats_per_vertex = 14;
        int length[4] = {4,3,4,4};
        int offset[4] = {0,3,6,10};

        for(int i = 0; i < 4; i++){
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, length[i], GL_FLOAT, GL_FALSE, floats_per_vertex * sizeof(float), BUFFER_OFFSET(offset[i] * sizeof(float)));

        }

    }

    int get_data_size(){
        int size = 0;
        size += positions.size() * (2 * sizeof(vec3) + 2 * sizeof(vec4));
        size += indices.size() * sizeof(uint16_t);
        return size;
    }

    static Mesh* get_cube();
    static Mesh* get_quad();
    static Mesh* get_sphere();
private:
    static Mesh* cube_mesh;
    static Mesh* quad_mesh;
    static Mesh* sphere_mesh;
};
