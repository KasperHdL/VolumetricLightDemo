
#include "Engine.hpp"

#include "../game/Game.hpp"
#include "utils/DebugInterface.hpp"



DynamicPool<Entity> Engine::entities = DynamicPool<Entity>(32);
Camera* Engine::camera;

Engine::Engine(int screen_width, int screen_height){
    this->screen_width = screen_width;
    this->screen_height = screen_height;
}

Engine::~Engine(){
}

int Engine::initialize(Game* game){
    this->game = game;

    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
            "An SDL2 window",                     // window title
            SDL_WINDOWPOS_UNDEFINED,              // initial x position
            SDL_WINDOWPOS_UNDEFINED,              // initial y position
            screen_width,                                  // width, in pixels
            screen_height,                                  // height, in pixels
            SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE// flags
    );

    // Check that the window was successfully made
    if (window == NULL) {
        // In the event that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    RenderEngine r{window};
    Engine::camera = r.getCamera();

    update_shaders();

    game->initialize(this);

    debug = new DebugInterface();
    debug->initialize(window, game);

    r.setAmbientLight({0.5,0.5,0.5});
    r.setLight(0, Light::create().withDirectionalLight(glm::vec3(.1f,.7f,.1f)).withColor(glm::vec3(1.0f,1.0f,1.0f)).build());

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;

    while (!input.quit){

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        delta_time = clamp(((NOW - LAST) / (float)SDL_GetPerformanceFrequency() ),0.0f,1.0f);
        time += delta_time;

        update(delta_time);
        render(delta_time);

        SDL_Delay(16.0f);
    }


    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();

    delete debug;

    return 0;
}


void Engine::update(float delta_time){
    input.update();
    Engine::camera->update();
    game->update(delta_time);
    debug->update(delta_time);
    if(debug->hotload_shader){
        update_shaders();
    }
}

void Engine::render(float delta_time){

    RenderEngine &r = *RenderEngine::instance;
    int w,h;
    SDL_GetWindowSize(window,&w,&h);

    camera->setViewport(0,0,w,h);
    camera->setPerspectiveProjection(60,w,h,0.1f,100);

    r.clearScreen({0.1f,0.1f,0.1f,1});
    //glBindFramebuffer(GL_FRAMEBUFFER, r.frame_buffer);


    for(int i = 0; i < entities.capacity;i++){
        Entity* e = entities[i];
        if(e != nullptr && e->mesh != nullptr){

            glm::mat4 s = glm::scale(mat4(), e->scale);
            glm::mat4 t = glm::translate(mat4(), e->position);
            glm::mat4 a = mat4_cast(e->rotation);

            r.draw(e->mesh,  t * s * a, _shader);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    debug->render(delta_time);

    r.swapWindow();
}

void Engine::update_shaders(){
    bool changed = false;

    struct stat st;
    int ierr = stat(DataPath::get("standard_vert.glsl").c_str(), &st);
    if (ierr != 0) {
            cout << "error";
    }

    if(st.st_mtime > _shader_time) changed = true;

    if(!changed){
        ierr = stat(DataPath::get("standard_frag.glsl").c_str(), &st);
        if (ierr != 0) {
                cout << "error";
        }
        if(st.st_mtime > _shader_time) changed = true;
    }

    if(st.st_mtime > _shader_time){
        cout << "Loading Shader\n";
        _shader_time = st.st_mtime;

        std::string vert = FileLoader::load_file_as_string("standard_vert.glsl");
        std::string frag = FileLoader::load_file_as_string("standard_frag.glsl");

        Shader* s = Shader::create().withSource(vert.c_str(), frag.c_str()).build();

        if(s != nullptr){
            cout << "Successfully loaded shader!\n";

            if(_shader != nullptr)
                delete _shader;

            _shader = s;

            _shader->set("color", glm::vec4(1));
            _shader->set("tex", Texture::getWhiteTexture());
        }else
            cout << "Encountered Error!\n";
    }
}

