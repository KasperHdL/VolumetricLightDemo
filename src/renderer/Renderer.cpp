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

