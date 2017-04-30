//
// Created by morten on 31/07/16.
//

#if defined(_WIN32) // force high performance graphics card see https://github.com/grimfang4/sdl-gpu/issues/17
#include <windows.h> // <---- for the DWORD
extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

#include "RenderEngine.hpp"
#include <cassert>
#include "Shader.hpp"
#include "Mesh.hpp"

#include "impl/GL.hpp"
#include <iostream>
#include <algorithm>
#include <string>

RenderEngine* RenderEngine::instance = nullptr;

RenderEngine::RenderEngine(SDL_Window * window)
:window{window}
{
    if (instance != nullptr){
        std::cerr << "Multiple versions of RenderEngine initialized. Only a single instance is supported." << std::endl;
    }
    instance = this;
    camera = &defaultCamera;
#ifndef EMSCRIPTEN
    glcontext = SDL_GL_CreateContext(window);
#endif
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    std::string version = (char*)glGetString(GL_VERSION);
    std::cout << "OpenGL version "<< version << std::endl;
    std::cout << "Render Engine version " << version_major << "." << version_minor << "." << version_point << std::endl;

    // setup opengl context
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
#ifndef EMSCRIPTEN
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN,GL_LOWER_LEFT);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
#ifndef GL_POINT_SPRITE
#define GL_POINT_SPRITE 0x8861
#endif // !GL_POINT_SPRITE
    if (version.find_first_of("3.1") == 0){
        glEnable(GL_POINT_SPRITE);
    }
    SDL_GetWindowSize(window,&camera->viewportWidth,&camera->viewportHeight);
    
// reset render stats
    renderStats.frame = 0;
    renderStats.meshCount = 0;
    renderStats.meshBytes = 0;
    renderStats.textureCount = 0;
    renderStats.textureBytes = 0;
    renderStats.drawCalls = 0;
    renderStatsLast = renderStats;
}

RenderEngine::~RenderEngine() {

    SDL_GL_DeleteContext(glcontext);
    instance = nullptr;
}

void RenderEngine::setLight(int lightIndex, Light light) {
    assert(lightIndex >= 0);
    assert(lightIndex < maxSceneLights);
    sceneLights[lightIndex] = light;
}

Light RenderEngine::getLight(int lightIndex) {
    assert(lightIndex >= 0);
    assert(lightIndex < maxSceneLights);
    return sceneLights[lightIndex];
}

void RenderEngine::draw(Mesh *mesh, glm::mat4 modelTransform, Shader *shader) {
    renderStats.drawCalls++;
    if (camera == nullptr){
        std::cerr<<"Cannot render. Camera is null"<<std::endl;
        return;
    }
    setupShader(modelTransform, shader);

    mesh->bind();
    int indexCount = (int) mesh->getIndices().size();
    if (indexCount == 0){
        glDrawArrays((GLenum) mesh->getMeshTopology(), 0, mesh->getVertexCount());
    } else {
        glDrawElements((GLenum) mesh->getMeshTopology(), indexCount, GL_UNSIGNED_SHORT, 0);
    }
}

void RenderEngine::setupShader(const glm::mat4 &modelTransform, Shader *shader)  {
    shader->bind();
    if (shader->getType("model").type != UniformType::Invalid) {
        shader->set("model", modelTransform);
    }
    if (shader->getType("view").type != UniformType::Invalid) {
        shader->set("view", camera->getViewTransform());
    }
    if (shader->getType("projection").type != UniformType::Invalid) {
        shader->set("projection", camera->getProjectionTransform());
    }
    if (shader->getType("normalMat").type != UniformType::Invalid){
        auto normalMatrix = transpose(inverse((glm::mat3)(camera->getViewTransform() * modelTransform)));
        shader->set("normalMat", normalMatrix);
    }
    if (shader->getType("view_height").type != UniformType::Invalid){
        shader->set("view_height", (float)camera->viewportHeight);
    }

    shader->setLights(sceneLights, ambientLight, camera->getViewTransform());
}

void RenderEngine::setCamera(Camera *camera) {
    this->camera = camera;
    camera->setViewport(camera->viewportX, camera->viewportY, camera->viewportWidth, camera->viewportHeight);
}

void RenderEngine::clearScreen(glm::vec4 color, bool clearColorBuffer, bool clearDepthBuffer) {
    glClearColor(color.r, color.g, color.b, color.a);
    GLbitfield clear = 0;
    if (clearColorBuffer){
        clear |= GL_COLOR_BUFFER_BIT;
    }
    if (clearDepthBuffer){
        glDepthMask(GL_TRUE);
        clear |= GL_DEPTH_BUFFER_BIT;
    }
    glClear(clear);
}

void RenderEngine::swapWindow() {
    renderStatsLast = renderStats;
    renderStats.frame++;
    renderStats.drawCalls=0;

    SDL_GL_SwapWindow(window);
}

Camera *RenderEngine::getCamera() {
    return camera;
}

Camera *RenderEngine::getDefaultCamera() {
    return &defaultCamera;
}

glm::vec3 RenderEngine::getAmbientLight() const {
    return glm::vec3(ambientLight);
}

void RenderEngine::setAmbientLight(const glm::vec3 &ambientLight) {
    float maxAmbient = std::max(ambientLight.x, std::max(ambientLight.y,ambientLight.z));
    RenderEngine::ambientLight = glm::vec4(ambientLight, maxAmbient);
}

void RenderEngine::finishGPUCommandBuffer() {
    glFinish();
}

const RenderStats &RenderEngine::getRenderStats() {
    return renderStatsLast;
}

