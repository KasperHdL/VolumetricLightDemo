#include "Mesh.hpp"

//Helper Functions
Mesh* Mesh::cube_mesh = nullptr;

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

    return Mesh::cube_mesh;
}


