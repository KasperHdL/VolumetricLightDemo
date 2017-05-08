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

    //camera
    camera = new Camera();
    camera->set_viewport(0,0,screen_width, screen_height);
    camera->set_perspective_projection();

    //Shader setup
    {

        //debug
        debug_shader = AssetManager::get_shader("debug");

        debug_shader->use();
        debug_shader->init_uniform("model"        , Shader::Uniform_Type::Mat4);
        debug_shader->init_uniform("view"         , Shader::Uniform_Type::Mat4);
        debug_shader->init_uniform("projection"   , Shader::Uniform_Type::Mat4);
        debug_shader->init_uniform("color"        , Shader::Uniform_Type::Vec4);

        //depth
        depth_shader = AssetManager::get_shader("depth");

        depth_shader->use();
        depth_shader->init_uniform("model"       , Shader::Uniform_Type::Mat4);
        depth_shader->init_uniform("light_index" , Shader::Uniform_Type::Int);
        depth_shader->init_uniform("color"       , Shader::Uniform_Type::Vec4);
        depth_shader->init_uniform("shadow_vp"   , Shader::Uniform_Type::Mat4);


        //deferred
        shader = AssetManager::get_shader("deferred");

        shader->use();
        shader->init_uniform("model"        , Shader::Uniform_Type::Mat4);
        shader->init_uniform("view"         , Shader::Uniform_Type::Mat4);
        shader->init_uniform("projection"   , Shader::Uniform_Type::Mat4);
        shader->init_uniform("color"        , Shader::Uniform_Type::Vec4);


        //screen 
        screen_shader = AssetManager::get_shader("screen");

        screen_shader->init_uniform("camera_position"  , Shader::Uniform_Type::Vec4);
        screen_shader->init_uniform("position_texture" , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("local_position_texture" , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("normal_texture"   , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("color_texture"    , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("num_lights"       , Shader::Uniform_Type::Int);
        screen_shader->init_uniform("shadow_vp"        , Shader::Uniform_Type::Mat4);
        screen_shader->init_uniform("shadow_map"       , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("inverse_mvp"      , Shader::Uniform_Type::Texture);


        const int max_lights = 10;
        for(int i = 0; i < max_lights; i++){
            string l = "lights[" + to_string(i) + "].";
            screen_shader->init_uniform(l + "position"    , Shader::Uniform_Type::Vec4);
            screen_shader->init_uniform(l + "color"       , Shader::Uniform_Type::Vec4);
            screen_shader->init_uniform(l + "attenuation" , Shader::Uniform_Type::Vec4);
            screen_shader->init_uniform(l + "cone"        , Shader::Uniform_Type::Vec4);
        }
    }

    //depth framebuffer
    {
        
        //depth
        glGenTextures(1, &depth_texture);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

        GLfloat border[] = {1,1,1,1};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);


        glGenFramebuffers(1, &depth_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, depth_framebuffer);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

    }

    //deferred framebuffer
    {
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

        //local position
        glGenTextures(1, &local_position_texture);
        glBindTexture(GL_TEXTURE_2D, local_position_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_width, screen_height, 0, GL_RGB, GL_FLOAT, 0);

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
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, local_position_texture, 0);

        const GLenum draw_buffers[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2
        };

        glDrawBuffers(3, draw_buffers);

    }

}

Renderer::~Renderer(){
    SDL_GL_DeleteContext(glcontext);
    instance = nullptr;
}

void Renderer::render(float delta_time){

    int w,h;
    SDL_GetWindowSize(window,&w,&h);

    //Depth Pass

    glEnable(GL_DEPTH_TEST);

    glViewport(0,0,shadow_width, shadow_height);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_framebuffer);

    glClearColor(0,0,-999,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    depth_shader->use();

    glm::mat4 shadow_vp;

    for(int i = 0; i < 1;i++){//God::lights.capacity;i++){
        Light* l = God::lights[i];
        if(l != nullptr){
            mat4 proj = glm::perspectiveFov<float>(radians<float>(90), shadow_width, shadow_height, 0.1f, 100.0);
            //mat4 proj = glm::ortho<float>(-10,10,-10,10,-10,20);
            //glm::mat4 view = glm::lookAt(vec3(0,0,0), l->position, glm::vec3(0,1,0));
            glm::mat4 view = glm::lookAt(l->position, l->position + l->direction, glm::vec3(0,1,0));

            shadow_vp = proj * view;
            depth_shader->set_uniform("shadow_vp", shadow_vp);
            //depth_shader->set_uniform("light_index", i);

            //render scene
            _render_scene(depth_shader);

        }
    }


    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);


    camera->set_viewport(0,0,w,h);
    camera->set_perspective_projection(); 
    mat4 projection = camera->projection_transform;

    //calc view transform
    camera->view_transform = glm::lookAt(camera->entity->position, vec3(0,1,0), glm::vec3(0,1,0));

    //setup deferred shader
    shader->use();

    shader->set_uniform("view"       , camera->view_transform);
    shader->set_uniform("projection" , projection);

    _render_scene(shader);

    //set uniforms
    time += delta_time;

    //setup screen shader
    screen_shader->use();
    screen_shader->set_uniform("camera_position", vec4(camera->entity->position,1));

    screen_shader->set_uniform("position_texture" , position_texture , 0);
    screen_shader->set_uniform("normal_texture"   , normal_texture   , 1);
    screen_shader->set_uniform("color_texture"    , color_texture    , 2);
    screen_shader->set_uniform("local_position_texture"    , local_position_texture    , 3);

    screen_shader->set_uniform("shadow_map"       , depth_texture    , 3);
    screen_shader->set_uniform("shadow_vp"        , shadow_vp);

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

    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(  0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    debug_shader->use();

    debug_shader->set_uniform("view"       , camera->view_transform);
    debug_shader->set_uniform("projection" , projection);

    //debug draw light

    if(debug->draw_light_pos){
        for(int i = 0; i < God::lights.capacity;i++){
            Light* l = God::lights[i];
            if(l != nullptr){
                //set uniforms

                glm::mat4 t = glm::translate(mat4(), l->position);
                glm::mat4 s = glm::scale(mat4(), vec3(.2f));
                glm::mat4 model = t * s;

                shader->set_uniform("model", model);
                shader->set_uniform("color", vec4(l->color,1));

                //draw mesh
                Mesh* mesh;

                if(l->type == Light::Type::Directional){
                    mesh = Mesh::get_quad();
                }else if(l->type == Light::Type::Point){
                    mesh = Mesh::get_sphere();
                }else if(l->type == Light::Type::Spot){
                    mesh = Mesh::get_cube();
                }

                mesh->bind();

                int indexCount = (int) mesh->indices.size();
                if (indexCount == 0){
                    glDrawArrays((GLenum)mesh->topology, 0, mesh->vertex_count);
                } else {
                    glDrawElements((GLenum) mesh->topology, indexCount, GL_UNSIGNED_SHORT, 0);
                }
            }
        }
    }


    debug->render(delta_time);

    SDL_GL_SwapWindow(window);

    return;
}

void Renderer::_render_scene(Shader* shader){
    //draw scene
    for(int i = 0; i < God::entities.capacity;i++){
        Entity* e = God::entities[i];
        if(e != nullptr && e->mesh != nullptr){
            //set uniforms

            glm::mat4 t = glm::translate(mat4(), e->position);
            glm::mat4 s = glm::scale(mat4(), e->scale);
            glm::mat4 a = glm::eulerAngleYXZ(e->rotation.x,e->rotation.y,e->rotation.z);


            glm::mat4 model_transform = t * a * s;

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

}

