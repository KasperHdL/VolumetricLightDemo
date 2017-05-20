#include "Mesh.hpp"
#include "glm/gtc/constants.hpp"


//Helper Functions
Mesh* Mesh::cube_mesh = nullptr;
Mesh* Mesh::quad_mesh = nullptr;
Mesh* Mesh::sphere_mesh = nullptr;

Mesh* Mesh::get_cube(){
    if(cube_mesh != nullptr)
        return cube_mesh;

    float length = 1.0f;
    //    v5----- v4
    //   /|      /|
    //  v1------v0|
    //  | |     | |
    //  | |v6---|-|v7
    //  |/      |/
    //  v2------v3
    vec3 p[] = {
            vec3{length, length, length},
            vec3{-length, length, length},
            vec3{-length, -length, length},
            vec3{length, -length, length},

            vec3{length, length, -length},
            vec3{-length, length, -length},
            vec3{-length, -length, -length},
            vec3{length, -length, -length}

    };
    vector<vec3> positions({p[0],p[1],p[2], p[0],p[2],p[3], // v0-v1-v2-v3
                            p[4],p[0],p[3], p[4],p[3],p[7], // v4-v0-v3-v7
                            p[5],p[4],p[7], p[5],p[7],p[6], // v5-v4-v7-v6
                            p[1],p[5],p[6], p[1],p[6],p[2], // v1-v5-v6-v2
                            p[4],p[5],p[1], p[4],p[1],p[0], // v1-v5-v6-v2
                            p[3],p[2],p[6], p[3],p[6],p[7], // v1-v5-v6-v2
                           });
    vec4 u[] = {
            vec4(1,1,0,0),
            vec4(0,1,0,0),
            vec4(0,0,0,0),
            vec4(1,0,0,0)
    };
    vector<vec4> uvs({ u[0],u[1],u[2], u[0],u[2],u[3],
                       u[0],u[1],u[2], u[0],u[2],u[3],
                       u[0],u[1],u[2], u[0],u[2],u[3],
                       u[0],u[1],u[2], u[0],u[2],u[3],
                       u[0],u[1],u[2], u[0],u[2],u[3],
                       u[0],u[1],u[2], u[0],u[2],u[3],
                     });
    vector<vec3> normals({
                                 vec3{0, 0, 1},
                                 vec3{0, 0, 1},
                                 vec3{0, 0, 1},
                                 vec3{0, 0, 1},
                                 vec3{0, 0, 1},
                                 vec3{0, 0, 1},
                                 vec3{1, 0, 0},
                                 vec3{1, 0, 0},
                                 vec3{1, 0, 0},
                                 vec3{1, 0, 0},
                                 vec3{1, 0, 0},
                                 vec3{1, 0, 0},
                                 vec3{0, 0, -1},
                                 vec3{0, 0, -1},
                                 vec3{0, 0, -1},
                                 vec3{0, 0, -1},
                                 vec3{0, 0, -1},
                                 vec3{0, 0, -1},
                                 vec3{-1, 0, 0},
                                 vec3{-1, 0, 0},
                                 vec3{-1, 0, 0},
                                 vec3{-1, 0, 0},
                                 vec3{-1, 0, 0},
                                 vec3{-1, 0, 0},
                                 vec3{0, 1, 0},
                                 vec3{0, 1, 0},
                                 vec3{0, 1, 0},
                                 vec3{0, 1, 0},
                                 vec3{0, 1, 0},
                                 vec3{0, 1, 0},
                                 vec3{0, -1, 0},
                                 vec3{0, -1, 0},
                                 vec3{0, -1, 0},
                                 vec3{0, -1, 0},
                                 vec3{0, -1, 0},
                                 vec3{0, -1, 0},
                         });
    vector<vec4> colors;
    vector<uint16_t> indices;

    Mesh::cube_mesh = new Mesh(positions, normals, uvs, colors, indices);

    Mesh::cube_mesh->name = "Cube";
    return Mesh::cube_mesh;
}

Mesh* Mesh::get_quad(){
    if(Mesh::quad_mesh != nullptr)
        return Mesh::quad_mesh;

    std::vector<glm::vec3> vertices({
                                            glm::vec3{1, -1, 0},
                                            glm::vec3{1, 1, 0},
                                            glm::vec3{-1, -1, 0},
                                            glm::vec3{-1, 1, 0}
                                    });
    std::vector<glm::vec3> normals({
                                           glm::vec3{0, 0, 1},
                                           glm::vec3{0, 0, 1},
                                           glm::vec3{0, 0, 1},
                                           glm::vec3{0, 0, 1}
                                   });
    std::vector<glm::vec4> uvs({
                                       glm::vec4{1, 0,0,0},
                                       glm::vec4{1, 1,0,0},
                                       glm::vec4{0, 0,0,0},
                                       glm::vec4{0, 1,0,0}
                               });
    vector<vec4> colors;
    std::vector<uint16_t> indices = {
            0,1,2,
            2,1,3
    };
    Mesh::quad_mesh = new Mesh(vertices, normals, uvs , colors, indices);

    Mesh::quad_mesh->name = "Quad";
    return Mesh::quad_mesh;

}


Mesh* Mesh::get_sphere(){
    if(Mesh::sphere_mesh != nullptr)
        return Mesh::sphere_mesh;

    using namespace glm;
    using namespace std;
    int stacks = 16/2;
    int slices = 32/2;
    float radius = 1.0f;
    size_t vertexCount = (size_t) ((stacks + 1) * slices);
    vector<vec3> vertices{vertexCount};
    vector<vec3> normals{vertexCount};
    vector<vec4> uvs{vertexCount};

    int index = 0;
    // create vertices
    for (unsigned short j = 0; j <= stacks; j++) {
        float latitude1 = (glm::pi<float>() / stacks) * j - (glm::pi<float>() / 2);
        float sinLat1 = sin(latitude1);
        float cosLat1 = cos(latitude1);
        for (int i = 0; i < slices; i++) {
            float longitude = ((glm::pi<float>() * 2) / slices) * i;
            float sinLong = sin(longitude);
            float cosLong = cos(longitude);
            vec3 normal{cosLong * cosLat1,
                        sinLat1,
                        sinLong * cosLat1};
            normal = normalize(normal);
            normals[index] = normal;
            uvs[index] = vec4{1 - i /(float) slices, j /(float) stacks,0,0};
            vertices[index] = normal * radius;
            index++;
        }
    }
    vector<vec3> finalPosition;
    vector<vec3> finalNormals;
    vector<vec4> finalUVs;
    // create indices
    for (int j = 0; j < stacks; j++) {
        for (int i = 0; i < slices; i++) {
            glm::u8vec2 offset [] = {
                    // first triangle
                    glm::u8vec2{i,j},
                    glm::u8vec2{(i+1)%slices,j+1},
                    glm::u8vec2{(i+1)%slices,j},

                    // second triangle
                    glm::u8vec2{i,j},
                    glm::u8vec2{i,j+1},
                    glm::u8vec2{(i+1)%slices,j+1},

            };
            for (auto o : offset){
                index = o[1] * slices  + o[0];
                finalPosition.push_back(vertices[index]);
                finalNormals.push_back(normals[index]);
                finalUVs.push_back(uvs[index]);
            }

        }
    }
    vector<vec4> colors;
    vector<uint16_t> indices;

    Mesh::sphere_mesh = new Mesh(finalPosition, finalNormals, finalUVs, colors, indices);
    Mesh::sphere_mesh->name = "Sphere";

    return Mesh::sphere_mesh;

}
