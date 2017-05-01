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

    unsigned int shader_id;
    {
        std::string vert = FileLoader::load_file_as_string("standard_vert.glsl");
        std::string frag = FileLoader::load_file_as_string("standard_frag.glsl");
        shader_id = compile_shader(vert.c_str(), frag.c_str());
    }

}

Renderer::~Renderer(){
    SDL_GL_DeleteContext(glcontext);
    instance = nullptr;
}

void Renderer::render(){
    SDL_GL_SwapWindow(window);
}


unsigned int Renderer::compile_shader(const char* vertex_shader, const char* fragment_shader){
    unsigned int shader_id = glCreateProgram();

    std::vector<GLenum> shaderTypes{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};


    //Vertex Shader
    GLuint shader = glCreateShader(GL_VERTEX_SHADER);
    GLint length = (GLint) strlen(vertex_shader);

    glShaderSource(shader, 1, &vertex_shader, &length);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(success == GL_FALSE){
        //print errors
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        std::vector<char> errorLog((unsigned long) log_length);
        glGetShaderInfoLog(shader, log_length, &log_length, errorLog.data());

        std::string type_string = "Vertex Shader";

        std::cerr<<(std::string{errorLog.data()}+"\n"+ type_string +" error\n")<<std::endl;
    }

    glAttachShader(shader_id, shader);


    glLinkProgram(shader_id);

    GLint  linked;
    glGetProgramiv(shader_id, GL_LINK_STATUS, &linked );
    if (linked == GL_FALSE) {
        GLint  log_length;
        glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<char> errorLog((size_t) log_length);
        glGetProgramInfoLog(shader_id, log_length, NULL, errorLog.data() );

        std::cerr << (errorLog.data()) << std::endl;
    }

    return shader_id;
}
