#include "AssetManager.hpp"


DynamicPool<Shader> AssetManager::shader_pool = DynamicPool<Shader>(8);
vector<Mesh*> AssetManager::mesh_pool;

Shader* AssetManager::get_shader(string vertex_path, string fragment_path){
    string vert = FileLoader::load_file_as_string(vertex_path);
    string frag = FileLoader::load_file_as_string(fragment_path);

    Shader* shader = new (shader_pool.create()) Shader(vert, frag);

#if DEBUG

    if(shader->compiled == false)
        std::cout << "In either " << vertex_path << " or " << fragment_path << "\n\n";

    int timestamp;
    shader->vertex_path = vertex_path;
    shader->fragment_path = fragment_path;

    struct stat st;
    int ierr = stat(DataPath::get(vertex_path).c_str(), &st);
    if (ierr != 0){
        cout << "AssetManager could not stat file " << vertex_path << "\n";
        return shader;
    }

    timestamp = st.st_mtime;

    ierr = stat(DataPath::get(fragment_path).c_str(), &st);
    if (ierr != 0){
        cout << "AssetManager could not stat file " << fragment_path << "\n";
        return shader;
    }

    if(timestamp < st.st_mtime) timestamp = st.st_mtime;

    shader->timestamp = timestamp;

#endif

    return shader;
}

Shader* AssetManager::get_shader(string shader_path){
    return AssetManager::get_shader(shader_path + "_vert.glsl", shader_path + "_frag.glsl");
}


Mesh* AssetManager::get_mesh(string name){
    Mesh* mesh = ObjParser::load(name);
    if(mesh == nullptr) return nullptr;
    mesh->name = name;
    mesh_pool.push_back(mesh);

    return mesh;
}



void AssetManager::update(){
    for(int i = 0; i < AssetManager::shader_pool.capacity;i++)
        _check_shader(AssetManager::shader_pool[i]);
}

void AssetManager::cleanup(){
    for(int i = 0; i < AssetManager::shader_pool.capacity;i++){
        Shader* s = AssetManager::shader_pool[i];
        if(s != nullptr){
            AssetManager::shader_pool.remove(s);
            s->~Shader();
        }
    }

    for(int i = 0; i < mesh_pool.size();i++){
        delete mesh_pool[i];

    }

}



void AssetManager::_check_shader(Shader* shader){
    if(shader == nullptr) return;

    bool changed = false;

    struct stat st;
    int ierr = stat(DataPath::get(shader->vertex_path).c_str(), &st);
    if (ierr != 0) {
            cout << "error";
    }

    if(st.st_mtime > shader->timestamp) changed = true;

    if(!changed){
        ierr = stat(DataPath::get(shader->fragment_path).c_str(), &st);
        if (ierr != 0) {
                cout << "error";
        }
        if(st.st_mtime > shader->timestamp) changed = true;
    }

    if(st.st_mtime > shader->timestamp){
        cout << "Loading Shader\n";
        shader->timestamp = st.st_mtime;

        std::string vert = FileLoader::load_file_as_string(shader->vertex_path);
        std::string frag = FileLoader::load_file_as_string(shader->fragment_path);

        shader->recompile(vert, frag);

        if(shader != nullptr && shader->compiled){
            cout << "Successfully loaded shader!\n";
        }else{
            std::cout << "Error: In either " << shader->vertex_path << " or " << shader->fragment_path << "\n";
        }
    } 

}
