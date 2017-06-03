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


        //depth
        shadow_map_shader = AssetManager::get_shader("pass_vert.glsl","empty_frag.glsl");

        shadow_map_shader->use();
        shadow_map_shader->init_uniform("model"       , Shader::Uniform_Type::Mat4);
        shadow_map_shader->init_uniform("color"       , Shader::Uniform_Type::Vec4); //because it is using _render_scene
        shadow_map_shader->init_uniform("shadow_index" , Shader::Uniform_Type::Int); //note used
        shadow_map_shader->init_uniform("vp"   , Shader::Uniform_Type::Mat4);


        //geometry
        geom_shader = AssetManager::get_shader("geom");

        geom_shader->use();
        geom_shader->init_uniform("model"        , Shader::Uniform_Type::Mat4);
        geom_shader->init_uniform("view"         , Shader::Uniform_Type::Mat4);
        geom_shader->init_uniform("projection"   , Shader::Uniform_Type::Mat4);
        geom_shader->init_uniform("color"        , Shader::Uniform_Type::Vec4);


        //stencil
        stencil_shader = AssetManager::get_shader("pass_vert.glsl", "empty_frag.glsl");

        stencil_shader->use();
        stencil_shader->init_uniform("model" , Shader::Uniform_Type::Mat4);
        stencil_shader->init_uniform("vp"    , Shader::Uniform_Type::Mat4);


        //Light
        light_shader = AssetManager::get_shader("light");

        light_shader->init_uniform("mvp"                , Shader::Uniform_Type::Mat4);
        light_shader->init_uniform("screen_size"        , Shader::Uniform_Type::Vec4);

        light_shader->init_uniform("position_texture"   , Shader::Uniform_Type::Texture);
        light_shader->init_uniform("normal_texture"     , Shader::Uniform_Type::Texture);
        light_shader->init_uniform("color_texture"      , Shader::Uniform_Type::Texture);
        light_shader->init_uniform("specular_texture"   , Shader::Uniform_Type::Texture);

        light_shader->init_uniform("shadow_map"         , Shader::Uniform_Type::Texture);
        light_shader->init_uniform("camera_position"    , Shader::Uniform_Type::Vec4);

        light_shader->init_uniform("ray_att"            , Shader::Uniform_Type::Vec4);

        light_shader->init_uniform("light_position"     , Shader::Uniform_Type::Vec4);
        light_shader->init_uniform("light_color"        , Shader::Uniform_Type::Vec4);
        light_shader->init_uniform("light_attenuation"  , Shader::Uniform_Type::Vec4);
        light_shader->init_uniform("light_cone"         , Shader::Uniform_Type::Vec4);
        light_shader->init_uniform("light_shadow_vp"    , Shader::Uniform_Type::Mat4);
        light_shader->init_uniform("light_shadow_index" , Shader::Uniform_Type::Int);
        light_shader->init_uniform("time"               , Shader::Uniform_Type::Float);
        light_shader->init_uniform("color_rand"        , Shader::Uniform_Type::Float);


        //screen
        screen_shader = AssetManager::get_shader("screen");

        screen_shader->init_uniform("camera_position"    , Shader::Uniform_Type::Vec4);
        screen_shader->init_uniform("position_texture"   , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("normal_texture"     , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("color_texture"      , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("specular_texture"   , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("shadow_map"      , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("screen_texture"     , Shader::Uniform_Type::Texture);

        screen_shader->init_uniform("fog"                , Shader::Uniform_Type::Vec4);
        screen_shader->init_uniform("time"               , Shader::Uniform_Type::Float);
        screen_shader->init_uniform("position_rand"      , Shader::Uniform_Type::Float);

        screen_shader->init_uniform("light_direction"    , Shader::Uniform_Type::Vec4);
        screen_shader->init_uniform("light_color"        , Shader::Uniform_Type::Vec4);

        screen_shader->init_uniform("shadow_map"         , Shader::Uniform_Type::Texture);
        screen_shader->init_uniform("light_shadow_vp"    , Shader::Uniform_Type::Mat4);
        screen_shader->init_uniform("light_shadow_index" , Shader::Uniform_Type::Int);

        for(int i = 0; i < 9; i++)
            screen_shader->init_uniform("kernel[" + to_string(i) + "]", Shader::Uniform_Type::Float);

        //debug
        debug_shader = AssetManager::get_shader("pass_vert.glsl","debug_frag.glsl");

        debug_shader->use();
        debug_shader->init_uniform("model"        , Shader::Uniform_Type::Mat4);
        debug_shader->init_uniform("vp"         , Shader::Uniform_Type::Mat4);
        debug_shader->init_uniform("color"        , Shader::Uniform_Type::Vec4);
    }

    //depth framebuffer
    {
        
        //depth
        glGenTextures(1, &depth_texture);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, shadow_width, shadow_height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, 0);

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

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_texture, 0);

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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_width, screen_height, 0, GL_RGB, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        //specular
        glGenTextures(1, &specular_texture);
        glBindTexture(GL_TEXTURE_2D, specular_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_width, screen_height, 0, GL_RGB, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


        // The depth buffer
        glGenRenderbuffers(1, &depth_renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_renderbuffer);


        //configure framebuffer
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, position_texture, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normal_texture, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, color_texture, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, specular_texture, 0);

        const GLenum draw_buffers[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3
        };

        glDrawBuffers(4, draw_buffers);

    }

    {
        //Post Processing

        glGenTextures(1, &post_texture);
        glBindTexture(GL_TEXTURE_2D, post_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_width, screen_height, 0, GL_RGB, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


        glGenFramebuffers(1, &post_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, post_framebuffer);

        const GLenum draw_buffers[] = {
            GL_COLOR_ATTACHMENT4
        };
        glDrawBuffers(1, draw_buffers);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, post_texture, 0);


    }

}

Renderer::~Renderer(){

    glDeleteFramebuffers(1, &framebuffer);
    glDeleteFramebuffers(1, &depth_framebuffer);
    glDeleteRenderbuffers(1, &depth_renderbuffer);

    SDL_GL_DeleteContext(glcontext);
    instance = nullptr;
}

void Renderer::render(float delta_time){

    int w,h;
    SDL_GetWindowSize(window,&w,&h);

    //set uniforms
    time += delta_time;

    ////////////////////////////////
    //Create Shadow map
    ////////////////////////////////

    debug->shadow_map_timer.start();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    int one_shadow_width = shadow_width / max_shadow_maps;
    int shadow_count = 0;

    glViewport(one_shadow_width * (shadow_count), 0, one_shadow_width * (shadow_count + 1), shadow_height);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_framebuffer);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shadow_map_shader->use();



    //shadow map 
    for(int i = 0; i < God::lights.capacity;i++){
        if(shadow_count >= max_shadow_maps)break;
        Light* l = God::lights[i];
        if(l != nullptr && l->create_shadow_map){
            mat4 proj;
            mat4 view;

            if(l->type == Light::Type::Spot){
                proj = glm::perspectiveFov<float>(radians<float>(l->field_of_view), shadow_width, shadow_height, l->near_plane, l->far_plane);
                view = glm::lookAt(l->position, l->position + l->direction, glm::vec3(0,1,0));
            }else if(l->type == Light::Type::Directional){
                proj = glm::ortho<float>(l->left_plane, l->right_plane, l->bottom_plane, l->top_plane, l->near_plane, l->far_plane);
                view = glm::lookAt(vec3(0,0,0), l->position, glm::vec3(0,1,0));
            }else{
                //ignore point lights
                continue;
            }

            glViewport(one_shadow_width * (shadow_count), 0, one_shadow_width * (shadow_count + 1), shadow_height);

            mat4 shadow_vp = proj * view;
            shadow_map_shader->set_uniform("vp", shadow_vp);
            shadow_map_shader->set_uniform("shadow_index", shadow_count);

            l->shadow_vp = shadow_vp;
            l->shadow_map_index = shadow_count++;

            //render scene
            _render_scene(shadow_map_shader);

        }
    }

    debug->shadow_map_timer.stop();

    ////////////////////////////////
    //Write to Opaque Objects to G-Buffer
    ////////////////////////////////

    debug->opaque_timer.start();
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);


    camera->set_viewport(0,0,w,h);
    camera->set_perspective_projection(); 

    //setup deferred shader
    geom_shader->use();

    geom_shader->set_uniform("view"       , camera->view_transform);
    geom_shader->set_uniform("projection" , camera->projection_transform);

    _render_scene(geom_shader);

    glDepthMask(GL_FALSE);
    debug->opaque_timer.stop();

    ////////////////////////////////
    //Stencil Light Calculation
    ////////////////////////////////

    debug->stencil_light_timer.start();
    glEnable(GL_STENCIL_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_CULL_FACE);
    glClear(GL_STENCIL_BUFFER_BIT);

    glStencilFunc(GL_ALWAYS, 0, 0);


    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    //

    stencil_shader->use();


    mat4 camera_vp = camera->projection_transform * camera->view_transform;

    for(int i = 0; i < God::lights.capacity;i++){
        Light* l = God::lights[i];
        if(l != nullptr && l->mesh != nullptr){
            if(l->type == Light::Type::Directional) continue;

            //render
            //@TODO(Kasper) no rotation currently
            glm::mat4 t = glm::translate(mat4(), l->position);
            glm::mat4 s = glm::scale(mat4(), l->scale);

            glm::mat4 model_transform = t * s;

            stencil_shader->set_uniform("vp", camera_vp);
            stencil_shader->set_uniform("model", model_transform);

            l->mesh->bind();

            int indexCount = (int) l->mesh->indices.size();
            if (indexCount == 0){
                glDrawArrays((GLenum)l->mesh->topology, 0, l->mesh->vertex_count);
            } else {
                glDrawElements((GLenum) l->mesh->topology, indexCount, GL_UNSIGNED_SHORT, 0);
            }

        }
    }
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    debug->stencil_light_timer.stop();


    ////////////////////////////////
    //Render Light Influence 
    ////////////////////////////////


    debug->light_calc_timer.start();
    glBindFramebuffer(GL_FRAMEBUFFER, post_framebuffer);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);


    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

   	glBlendEquation(GL_FUNC_ADD);
   	glBlendFunc(GL_ONE, GL_ONE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set uniforms
    light_shader->use();
    light_shader->set_uniform("camera_position", vec4(camera->entity->position,1));

    light_shader->set_uniform("position_texture" , position_texture , 0);
    light_shader->set_uniform("normal_texture"   , normal_texture   , 1);
    light_shader->set_uniform("color_texture"    , color_texture    , 2);
    light_shader->set_uniform("specular_texture" , specular_texture , 3);

    light_shader->set_uniform("shadow_map"       , depth_texture    , 5);
    light_shader->set_uniform("screen_size", vec4(screen_width, screen_height, 0, 0));
    light_shader->set_uniform("time", time);
    light_shader->set_uniform("color_rand", debug->light_color_rand);

    light_shader->set_uniform("ray_att", vec4(debug->ray_att, debug->ray_rand));


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

            light_shader->set_uniform("mvp", camera_vp * model_transform);

            l->mesh->bind();

            int indexCount = (int) l->mesh->indices.size();
            if (indexCount == 0){
                glDrawArrays((GLenum)l->mesh->topology, 0, l->mesh->vertex_count);
            } else {
                glDrawElements((GLenum) l->mesh->topology, indexCount, GL_UNSIGNED_SHORT, 0);
            }

        }
    }


    glDepthMask(GL_FALSE);
    glCullFace(GL_BACK);


    //Render Directional Light

    Mesh* mesh = Mesh::get_quad();
    mesh->bind();
    for(int i = 0; i < God::lights.capacity;i++){
        Light* l = God::lights[i];
        if(l != nullptr){
            if(l->type != Light::Type::Directional) continue;

            light_shader->set_uniform("light_position" , vec4(l->position, l->type));
            light_shader->set_uniform("light_color"    , vec4(l->color, l->intensity));

            if(l->create_shadow_map){
                light_shader->set_uniform("light_shadow_vp"    , l->shadow_vp);
                light_shader->set_uniform("light_shadow_index" , l->shadow_map_index);

            }else{
                light_shader->set_uniform("light_shadow_index" , -1);
            }

            light_shader->set_uniform("mvp", mat4(1));

            int indexCount = (int) mesh->indices.size();
            if (indexCount == 0){
                glDrawArrays((GLenum)mesh->topology, 0, mesh->vertex_count);
            } else {
                glDrawElements((GLenum) mesh->topology, indexCount, GL_UNSIGNED_SHORT, 0);
            }

        }
    }
    glDisable(GL_BLEND);
    debug->light_calc_timer.stop();

    ////////////////////////////////
    //Write to Screen
    ////////////////////////////////

    debug->post_processing_timer.start();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //set uniforms
    time += delta_time;

    //setup screen shader
    screen_shader->use();

    screen_shader->set_uniform("camera_position", vec4(camera->entity->position,1));
    screen_shader->set_uniform("position_texture" , position_texture , 0);
    screen_shader->set_uniform("normal_texture"   , normal_texture   , 1);
    screen_shader->set_uniform("color_texture"    , color_texture    , 2);
    screen_shader->set_uniform("specular_texture" , specular_texture , 4);
    screen_shader->set_uniform("screen_texture"   , post_texture     , 4);
    screen_shader->set_uniform("shadow_map"       , depth_texture    , 5);
    screen_shader->set_uniform("fog", vec4(debug->fog_color, debug->fog_intensity));
    screen_shader->set_uniform("time", time);
    screen_shader->set_uniform("position_rand", debug->position_rand);

    for(int i = 0; i < 9; i++)
        screen_shader->set_uniform("kernel[" + to_string(i) + "]", debug->kernel[i]);


    //draw screen

    int indexCount = (int) mesh->indices.size();
    if (indexCount == 0){
        glDrawArrays((GLenum)mesh->topology, 0, mesh->vertex_count);
    } else {
        glDrawElements((GLenum) mesh->topology, indexCount, GL_UNSIGNED_SHORT, 0);
    }

    debug->post_processing_timer.stop();


    ///////////////////////////////
    //Debug Draw for light sources
    ///////////////////////////////

    debug->debug_draw_timer.start();
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);


    //debug draw light
    if(debug->draw_light_pos){

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(  0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        debug_shader->use();

        debug_shader->set_uniform("vp"       , camera_vp);


        for(int i = 0; i < God::lights.capacity;i++){
            Light* l = God::lights[i];
            if(l != nullptr){
                //set uniforms

                glm::mat4 t = glm::translate(mat4(), l->position);
                glm::mat4 s = glm::scale(mat4(), vec3(.1f));
                glm::mat4 model = t * s;

                debug_shader->set_uniform("model", model);
                debug_shader->set_uniform("color", vec4(l->color,1));

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
    debug->debug_draw_timer.stop();


    ///////////////////////////////
    //Debug UI
    ///////////////////////////////

    debug->debug_ui_timer.start();
    debug->render(delta_time);
    debug->debug_ui_timer.stop();

    ///////////////////////////////
    //Swap Window
    ///////////////////////////////
    debug->swap_buffer_timer.start();
    SDL_GL_SwapWindow(window);
    debug->swap_buffer_timer.stop();

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
            shader->set_uniform("color", vec4(vec3(e->color), e->specularity));

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

