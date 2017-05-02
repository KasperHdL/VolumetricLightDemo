#pragma once

#include "glm/glm.hpp"
#include <SDL_video.h>

#include "impl/GL.hpp"
#include <iostream>
#include <vector>

class Shader{
public:
    bool compiled = false;
    unsigned int program_id;

    Shader(std::string vertex, std::string fragment){
       compiled = _compile_shader(vertex.c_str(), fragment.c_str());

       if(!compiled)
           glDeleteShader(program_id);
    }

    Shader(const char* vertex, const char* fragment){
       compiled = _compile_shader(vertex, fragment);

       if(!compiled)
           glDeleteShader(program_id);
    }

    ~Shader(){
        glDeleteShader(program_id);
    }


private:

    bool _compile_shader(const char* vertex_shader, const char* fragment_shader){
        program_id = glCreateProgram();

        //vertex
        GLuint shader = 0;
        if(!_compile_shader_part(GL_VERTEX_SHADER, vertex_shader, shader))
            return false;

        glAttachShader(program_id, shader);

        //fragment
        shader = 0;
        if(!_compile_shader_part(GL_FRAGMENT_SHADER, fragment_shader, shader))
            return false;

        glAttachShader(program_id, shader);

        //link
        glLinkProgram(program_id);

        GLint  linked;
        glGetProgramiv(program_id, GL_LINK_STATUS, &linked );
        if (linked == GL_FALSE) {
            GLint  log_length;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
            std::vector<char> errorLog((size_t) log_length);
            glGetProgramInfoLog(program_id, log_length, NULL, errorLog.data() );

            std::cerr << (errorLog.data()) << std::endl;
            return false;
        }

        return true;
    }

    bool _compile_shader_part(GLenum type, const char* code, GLuint& shader){
        shader = glCreateShader(type);

        GLint length = (GLint) strlen(code);

        glShaderSource(shader, 1, &code, &length);
        glCompileShader(shader);

        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if(success == GL_FALSE){
            //print errors
            GLint log_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

            std::vector<char> errorLog((unsigned long) log_length);
            glGetShaderInfoLog(shader, log_length, &log_length, errorLog.data());

            std::string type_string = "";
            switch(type){
                case GL_VERTEX_SHADER:
                    type_string = "Vertex shader";
                break;
                case GL_FRAGMENT_SHADER:
                    type_string = "Fragment shader";
                break;
            }

            std::cerr<<(std::string{errorLog.data()}+"\n"+ type_string +" error\n")<<std::endl;
            return false;
        }

        return true;

    }


};
