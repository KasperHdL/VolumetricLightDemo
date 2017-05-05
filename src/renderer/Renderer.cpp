#include "Renderer.hpp"
#include "../Engine.hpp"
#include "../utils/DebugInterface.hpp"
#include "Camera.hpp"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(){}

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



    glClearColor(.5f,.5f,.5f,1);

    camera = new Camera();
    camera->set_viewport(0,0,screen_width, screen_height);
    camera->set_perspective_projection();

    new (God::lights.create()) Light(Light::Type::Directional, vec3(-1,-1,1), vec3(1,0,0), 1);
    new (God::lights.create()) Light(Light::Type::Point, vec3(0,1,0), vec3(0,1,0), 1);
    new (God::lights.create()) Light(Light::Type::Point, vec3(0,0,-1), vec3(0,0,1), 1);

    c1 = new (God::entities.create()) Entity();
    c1->name = "Light Follower 1";
    c1->scale = vec3(0.1f,0.1f,1);
    c1->rotation = quat();
    c1->mesh = Mesh::get_cube();

    c2 = new (God::entities.create()) Entity();
    c2->name = "Light Follower 2";
    c2->scale = vec3(0.1f);
    c2->rotation = quat();
    c2->mesh = Mesh::get_cube();

    c3 = new (God::entities.create()) Entity();
    c3->name = "Light Follower 3";
    c3->scale = vec3(0.1f);
    c3->rotation = quat();
    c3->mesh = Mesh::get_cube();

    {
        shader = AssetManager::get_shader("standard");

        shader->use();
        shader->init_uniform("model"        , Shader::Uniform_Type::Mat4);
        shader->init_uniform("view"         , Shader::Uniform_Type::Mat4);
        shader->init_uniform("projection"   , Shader::Uniform_Type::Mat4);
        shader->init_uniform("color"        , Shader::Uniform_Type::Vec4);


        //screen 

        screen_shader = AssetManager::get_shader("screen");

        screen_shader->init_uniform("camera_position", Shader::Uniform_Type::Vec4);

        screen_shader->init_uniform("position_texture", Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("normal_texture", Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("color_texture", Shader::Uniform_Type::Texture);

        screen_shader->init_uniform("num_lights", Shader::Uniform_Type::Int);

        const int max_lights = 10;
        for(int i = 0; i < max_lights; i++){
            string l = "lights[" + to_string(i) + "].";
            screen_shader->init_uniform(l + "position"    , Shader::Uniform_Type::Vec4);
            screen_shader->init_uniform(l + "color"       , Shader::Uniform_Type::Vec4);
            screen_shader->init_uniform(l + "attenuation" , Shader::Uniform_Type::Vec4);
            screen_shader->init_uniform(l + "cone"        , Shader::Uniform_Type::Vec4);
        }
    }

    //setup framebuffer

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    //create textures

    //position
    glGenTextures(1, &position_texture);
    glBindTexture(GL_TEXTURE_2D, position_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_width, screen_height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //normal
    glGenTextures(1, &normal_texture);
    glBindTexture(GL_TEXTURE_2D, normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_width, screen_height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //color
    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //depth
    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screen_width, screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // The depth buffer
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);


    //configure framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, position_texture, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normal_texture, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, color_texture, 0);

    GLenum draw_buffers[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };

    glDrawBuffers(3, draw_buffers);

    
}

Renderer::~Renderer(){
    SDL_GL_DeleteContext(glcontext);
    instance = nullptr;
}

void Renderer::render(float delta_time){

    int w,h;
    SDL_GetWindowSize(window,&w,&h);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glClearColor(.5f,.5f,.5f,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);


    camera->set_viewport(0,0,w,h);
    mat4 projection = glm::perspectiveFov<float>(radians<float>(60), (float)w, (float)h, 1, 100.0);
    camera->set_perspective_projection(); 

    //calc view transform
    glm::vec3 pos = 20.0f * vec3(sin(time), sin(time) * .2f + .3f, cos(time));
    camera->view_transform = glm::lookAt(camera->entity->position, vec3(0,1,0), glm::vec3(0,1,0));

    //setup deferred shader
    glUseProgram(shader->program_id);

    shader->set_uniform("view"       , camera->view_transform);
    shader->set_uniform("projection" , projection);
    
    //draw scene
    for(int i = 0; i < God::entities.capacity;i++){
        Entity* e = God::entities[i];
        if(e != nullptr && e->mesh != nullptr){
            //set uniforms

            glm::mat4 t = glm::translate(mat4(), e->position);
            glm::mat4 s = glm::scale(mat4(), e->scale);
            glm::mat4 a = mat4_cast(e->rotation); 
            glm::mat4 model_transform = t * s * a;

            shader->set_uniform("model", model_transform);
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

    float o= 0;
    h = 4;
    float ho = 2;
    float m = 5;

    pos = vec3(m * -sin(time + o), h + sin(time + ho), m * cos(time + o));
    //pos = vec3(m * -sin(0 + o), h + ho * sin(time + o), m * cos(0 + o));
//    God::lights[0]->position = pos;
 //   c1->position = pos;

    o= 2;
    ho = 2;
    h = 4;
    pos = vec3(m * -sin(time + o), h + sin(time + ho), m * cos(time + o));
    //pos = vec3(m * -sin(0 + o), h + ho * sin(time + o), m * cos(0 + o));
    God::lights[1]->position = pos;
    c2->position = pos;

    o= 4;
    ho = 2;
    h = 4;
    pos = vec3(m * -sin(time + o), h + sin(time + ho), m * cos(time + o));
    //pos = vec3(m * -sin(0 + o), h + ho * sin(time + o), m * cos(0 + o));
    God::lights[2]->position = pos;
    c3->position = pos;


    //set uniforms
    time += delta_time;
    //setup screen shader
    screen_shader->use();
    screen_shader->set_uniform("camera_position", vec4(camera->entity->position,1));

    screen_shader->set_uniform("position_texture" , position_texture , 0);
    screen_shader->set_uniform("normal_texture"   , normal_texture   , 1);
    screen_shader->set_uniform("color_texture"    , color_texture    , 2);

    //setup light

    screen_shader->set_uniform("num_lights"       , God::lights.count);
    for(int i = 0; i < God::lights.capacity;i++){
        Light* l = God::lights[i];
        if(l != nullptr){
            string name = "lights[" + to_string(i) + "].";
            screen_shader->set_uniform(name + "position" , vec4(l->position, l->type));
            screen_shader->set_uniform(name + "color"    , vec4(l->color, l->intensity));
            if(l->type >= Light::Type::Point){
                screen_shader->set_uniform(name + "attenuation" , vec4(l->attenuation , 1));
            }
            if(l->type == Light::Type::Spot)
                screen_shader->set_uniform(name + "cone"        , vec4(l->direction,l->cutoff));
        }
    }

    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,camera->viewport_w, camera->viewport_h);

    //draw screen
    Mesh* mesh = Mesh::get_quad();
    mesh->bind();

    int indexCount = (int) mesh->indices.size();
    if (indexCount == 0){
        glDrawArrays((GLenum)mesh->topology, 0, mesh->vertex_count);
    } else {
        glDrawElements((GLenum) mesh->topology, indexCount, GL_UNSIGNED_SHORT, 0);
    }

    debug->render(delta_time);

    SDL_GL_SwapWindow(window);

    return;
}

