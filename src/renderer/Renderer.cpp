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
        depth_shader->init_uniform("color"       , Shader::Uniform_Type::Vec4);
        depth_shader->init_uniform("shadow_index" , Shader::Uniform_Type::Int);
        depth_shader->init_uniform("shadow_vp"   , Shader::Uniform_Type::Mat4);


        //deferred
        shader = AssetManager::get_shader("deferred");

        shader->use();
        shader->init_uniform("model"        , Shader::Uniform_Type::Mat4);
        shader->init_uniform("view"         , Shader::Uniform_Type::Mat4);
        shader->init_uniform("projection"   , Shader::Uniform_Type::Mat4);
        shader->init_uniform("color"        , Shader::Uniform_Type::Vec4);


        //Light
        light_shader = AssetManager::get_shader("light");

        light_shader->init_uniform("mvp"                , Shader::Uniform_Type::Mat4);
        light_shader->init_uniform("screen_size"        , Shader::Uniform_Type::Vec4);

        light_shader->init_uniform("position_texture"   , Shader::Uniform_Type::Texture);
        light_shader->init_uniform("normal_texture"     , Shader::Uniform_Type::Texture);
        light_shader->init_uniform("color_texture"      , Shader::Uniform_Type::Texture);

        light_shader->init_uniform("shadow_map"         , Shader::Uniform_Type::Texture);
        light_shader->init_uniform("camera_position"    , Shader::Uniform_Type::Vec4);

        light_shader->init_uniform("light_position"     , Shader::Uniform_Type::Vec4);
        light_shader->init_uniform("light_color"        , Shader::Uniform_Type::Vec4);
        light_shader->init_uniform("light_attenuation"  , Shader::Uniform_Type::Vec4);
        light_shader->init_uniform("light_cone"         , Shader::Uniform_Type::Vec4);
        light_shader->init_uniform("light_shadow_vp"    , Shader::Uniform_Type::Mat4);
        light_shader->init_uniform("light_shadow_index" , Shader::Uniform_Type::Int);


        //screen
        screen_shader = AssetManager::get_shader("screen");

        screen_shader->init_uniform("position_texture" , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("normal_texture"   , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("color_texture"    , Shader::Uniform_Type::Texture);

        screen_shader->init_uniform("shadow_map"       , Shader::Uniform_Type::Texture);

        screen_shader->init_uniform("light_position"     , Shader::Uniform_Type::Vec4);
        screen_shader->init_uniform("light_color"        , Shader::Uniform_Type::Vec4);
        screen_shader->init_uniform("light_shadow_vp"    , Shader::Uniform_Type::Mat4);
        screen_shader->init_uniform("light_shadow_index" , Shader::Uniform_Type::Int);

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

    ////////////////////////////////
    //Create Shadow map
    ////////////////////////////////

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glViewport(0,0,shadow_width, shadow_height);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_framebuffer);

    glClearColor(0,0,-999,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    depth_shader->use();


    int shadow_count;

    //shadow map for sun

    if(sun != nullptr){
        Light* l = sun;

        mat4 proj = glm::ortho<float>(l->left_plane, l->right_plane, l->bottom_plane, l->top_plane, l->near_plane, l->far_plane);
        mat4 view = glm::lookAt(vec3(0,0,0), l->position, glm::vec3(0,1,0));

        mat4 shadow_vp = proj * view;
        depth_shader->set_uniform("shadow_vp", shadow_vp);
        depth_shader->set_uniform("shadow_index", shadow_count);

        l->shadow_vp = shadow_vp;
        l->shadow_map_index = shadow_count++;

        //render scene
        _render_scene(depth_shader);

    }

    //shadow map for other lights
    for(int i = 0; i < God::lights.capacity;i++){
        Light* l = God::lights[i];
        if(l != nullptr && l->create_shadow_map){
            mat4 proj;
            mat4 view;

            if(l->type == Light::Type::Spot){
                proj = glm::perspectiveFov<float>(radians<float>(l->field_of_view), shadow_width, shadow_height, l->near_plane, l->far_plane);
                view = glm::lookAt(l->position, l->position + l->direction, glm::vec3(0,1,0));
            }else{
                continue;

            }

            mat4 shadow_vp = proj * view;
            depth_shader->set_uniform("shadow_vp", shadow_vp);
            depth_shader->set_uniform("shadow_index", shadow_count);

            l->shadow_vp = shadow_vp;
            l->shadow_map_index = shadow_count++;

            //render scene
            _render_scene(depth_shader);

        }
    }


    ////////////////////////////////
    //Write to Opaque Objects to G-Buffer
    ////////////////////////////////

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);


    camera->set_viewport(0,0,w,h);
    camera->set_perspective_projection(); 

    //calc view transform
    camera->view_transform = glm::lookAt(camera->entity->position, vec3(0,1,0), glm::vec3(0,1,0));

    //setup deferred shader
    shader->use();

    shader->set_uniform("view"       , camera->view_transform);
    shader->set_uniform("projection" , camera->projection_transform);

    _render_scene(shader);


    ////////////////////////////////
    //Render Light Influence 
    ////////////////////////////////


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

   	glBlendEquation(GL_FUNC_ADD);
   	glBlendFunc(GL_ONE, GL_ONE);

    glClear(GL_COLOR_BUFFER_BIT);

    //set uniforms
    light_shader->use();
    light_shader->set_uniform("camera_position", vec4(camera->entity->position,1));

    light_shader->set_uniform("position_texture" , position_texture , 0);
    light_shader->set_uniform("normal_texture"   , normal_texture   , 1);
    light_shader->set_uniform("color_texture"    , color_texture    , 2);

    light_shader->set_uniform("shadow_map"       , depth_texture    , 3);
    light_shader->set_uniform("screen_size", vec4(screen_width, screen_height, 0, 0));


    mat4 vp = camera->projection_transform * camera->view_transform;

    for(int i = 0; i < God::lights.capacity;i++){
        Light* l = God::lights[i];
        if(l != nullptr && l->mesh != nullptr){
            if(l->type == Light::Type::Directional) continue;

            light_shader->set_uniform("light_position" , vec4(l->position, l->type));
            light_shader->set_uniform("light_color"    , vec4(l->color, l->intensity));

            light_shader->set_uniform("light_attenuation" , vec4(l->attenuation , 1));

            if(l->type == Light::Type::Spot)
                light_shader->set_uniform("light_cone" , vec4(l->direction , l->falloff));

            if(l->create_shadow_map){
                light_shader->set_uniform("light_shadow_vp"    , l->shadow_vp);
                light_shader->set_uniform("light_shadow_index" , l->shadow_map_index);

            }else{
                light_shader->set_uniform("light_shadow_index" , -1);
            }

            //render
            //@TODO(Kasper) no rotation currently
            glm::mat4 t = glm::translate(mat4(), l->position);
            glm::mat4 s = glm::scale(mat4(), l->scale);

            glm::mat4 model_transform = t * s;

            light_shader->set_uniform("mvp", vp * model_transform);

            l->mesh->bind();

            int indexCount = (int) l->mesh->indices.size();
            if (indexCount == 0){
                glDrawArrays((GLenum)l->mesh->topology, 0, l->mesh->vertex_count);
            } else {
                glDrawElements((GLenum) l->mesh->topology, indexCount, GL_UNSIGNED_SHORT, 0);
            }

        }
    }


    ////////////////////////////////
    //Write to Screen
    ////////////////////////////////

    /*

    //set uniforms
    time += delta_time;

    //setup screen shader
    screen_shader->use();

    screen_shader->set_uniform("position_texture" , position_texture , 0);
    screen_shader->set_uniform("normal_texture"   , normal_texture   , 1);
    screen_shader->set_uniform("color_texture"    , color_texture    , 2);

//    screen_shader->set_uniform("shadow_map"       , depth_texture    , 3);

    //setup sun






    //draw screen
    Mesh* mesh = Mesh::get_quad();
    mesh->bind();

    int indexCount = (int) mesh->indices.size();
    if (indexCount == 0){
        glDrawArrays((GLenum)mesh->topology, 0, mesh->vertex_count);
    } else {
        glDrawElements((GLenum) mesh->topology, indexCount, GL_UNSIGNED_SHORT, 0);
    }
    */



    ///////////////////////////////
    //Debug Draw for light sources
    ///////////////////////////////

    /*
    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(  0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);


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
    */

    glDisable(GL_BLEND);


    ///////////////////////////////
    //Debug UI
    ///////////////////////////////
    debug->render(delta_time);

    ///////////////////////////////
    //Swap Window
    ///////////////////////////////
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

