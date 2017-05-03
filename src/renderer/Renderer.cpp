#include "Renderer.hpp"
#include "../Engine.hpp"
#include "../utils/DebugInterface.hpp"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(){
}

void Renderer::initialize(SDL_Window* window, int screen_width, int screen_height){
    if (instance != nullptr){
        std::cerr << "Multiple versions of Renderer initialized. Only a single instance is supported." << std::endl;
    }

    instance = this;

    this->window        = window;
    this->screen_width  = screen_width;
    this->screen_height = screen_height;

    glcontext = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    if(err != GLEW_OK)
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));


    camera = new Camera();
    camera->set_viewport(0,0,screen_width, screen_height);
    camera->set_perspective_projection();

    {
        std::string vert = FileLoader::load_file_as_string("standard_vert.glsl");
        std::string frag = FileLoader::load_file_as_string("standard_frag.glsl");

        shader = new Shader(vert, frag);

        shader->use();
        shader->init_uniform("model"        , Shader::Uniform_Type::Mat4);
        shader->init_uniform("view"         , Shader::Uniform_Type::Mat4);
        shader->init_uniform("projection"   , Shader::Uniform_Type::Mat4);
        shader->init_uniform("normal_matrix" , Shader::Uniform_Type::Mat3);
        shader->init_uniform("color"        , Shader::Uniform_Type::Vec4);
    }

}

Renderer::~Renderer(){
    SDL_GL_DeleteContext(glcontext);
    instance = nullptr;
}

void Renderer::render(){

    int w,h;
    SDL_GetWindowSize(window,&w,&h);

    camera->set_viewport(0,0,w,h);
    camera->set_perspective_projection(); 
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //calc view transform
    glm::vec3 dir = glm::mat4_cast(camera->entity->rotation) * glm::vec4(0,0,1,0);
    camera->view_transform = glm::lookAt(camera->entity->position, camera->entity->position + dir, glm::vec3(0,1,0));

    //setup deferred shader
    glUseProgram(shader->program_id);

    shader->set_uniform("view"       , camera->view_transform);
    shader->set_uniform("projection" , camera->projection_transform);
    //draw scene
    for(int i = 0; i < Engine::entities.capacity;i++){
        Entity* e = Engine::entities[i];
        if(e != nullptr && e->mesh != nullptr){
            //set uniforms

            glm::mat4 t = glm::translate(mat4(), e->position);
            glm::mat4 s = glm::scale(mat4(), e->scale);
            glm::mat4 a = mat4_cast(e->rotation); 
            glm::mat4 model_transform = t * s * a;

            shader->set_uniform("model", model_transform);

            mat3 normal_matrix = transpose(inverse((mat3)(camera->view_transform * model_transform)));
            shader->set_uniform("normal_matrix" , normal_matrix);
            shader->set_uniform("color", vec4(1,1,1,1));

            //draw mesh
            e->mesh->bind();

            int indexCount = (int) e->mesh->indices.size();
            if (indexCount == 0){
                glDrawArrays((GLenum)e->mesh->topology, 0, e->mesh->vertex_count);
            } else {
                glDrawElements((GLenum) e->mesh->topology, indexCount, GL_UNSIGNED_SHORT, 0);
            }
        }
    }

    //setup screen shader

    //set uniforms

    //draw screen

    debug->render(.16f);
    SDL_GL_SwapWindow(window);
    return;
}

