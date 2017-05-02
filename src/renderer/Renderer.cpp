#include "Renderer.hpp"


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


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    camera = new Camera();
    camera->set_viewport(0,0,screen_width, screen_height);
    camera->set_perspective_projection();

    {
        std::string vert = FileLoader::load_file_as_string("standard_vert.glsl");
        std::string frag = FileLoader::load_file_as_string("standard_frag.glsl");
        shader = new Shader(vert, frag);
    }

    mesh = Mesh::get_cube();

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

    glm::vec3 dir = glm::mat4_cast(camera->entity->rotation) * glm::vec4(0,0,1,0);
    camera->view_transform = glm::lookAt(camera->entity->position, camera->entity->position + dir, glm::vec3(0,1,0));

    //setup deferred shader
    glUseProgram(shader->program_id);
    glEnable(GL_DEPTH_TEST);

    //set uniforms
    
    //draw scene
        //sort mesh

    mesh->bind();

    //setup screen shader

    //set uniforms

    //draw screen

    SDL_GL_SwapWindow(window);
}

