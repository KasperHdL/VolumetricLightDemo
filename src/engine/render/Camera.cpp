//
// Created by morten on 01/08/16.
//

#include "Camera.hpp"

#include "impl/GL.hpp"

#include "RenderEngine.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../Engine.hpp"


Camera::Camera()
: viewTransform{1.0f},projectionTransform {1.0f}, viewportX{0}, viewportY{0}
{
    if (RenderEngine::instance){
        SDL_GetWindowSize(RenderEngine::instance->window,&viewportWidth,&viewportHeight);
    }

    entity = new (Engine::entities.create()) Entity();
    entity->name = "Camera";
    entity->mesh = nullptr;
    entity->position = glm::vec3();
    entity->scale = glm::vec3(1,1,1);
    entity->rotation = glm::quat();
}

void Camera::update(){
    glm::vec3 dir = glm::mat4_cast(entity->rotation) * glm::vec4(0,0,1,0);
    viewTransform = glm::lookAt(entity->position, entity->position + dir, glm::vec3(0,1,0)); 
}

void Camera::setPerspectiveProjection(float fieldOfViewY, float viewportWidth,float viewportHeight, float nearPlane, float farPlane) {
    projectionTransform = glm::perspectiveFov<float>	(	glm::radians( fieldOfViewY),
                                                      viewportWidth,
                                                      viewportHeight,
                                                      nearPlane,
                                                      farPlane);
}

void Camera::setOrthographicProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    projectionTransform = glm::ortho<float>	(left, right, bottom, top, nearPlane, farPlane);
}

void Camera::setWindowCoordinates(int width, int height){
    int w,h;
    SDL_GetWindowSize(RenderEngine::instance->window,&w,&h);
    if (width == -1){
        width = w;
    }
    if (height == -1){
        height = h;
    }
    setOrthographicProjection(0.0f,(float)width,0.0f,(float)height,1.0f,-1.0f);
    viewTransform = glm::mat4(1);
}


void Camera::lookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up) {
    viewTransform = glm::lookAt<float>(eye, at, up);
}

glm::mat4 Camera::getViewTransform() {
    return viewTransform;
}

glm::mat4 Camera::getProjectionTransform() {
    return projectionTransform;
}

void Camera::setViewTransform(const glm::mat4 &viewTransform) {
    Camera::viewTransform = viewTransform;
}

void Camera::setProjectionTransform(const glm::mat4 &projectionTransform) {
    Camera::projectionTransform = projectionTransform;
}

void Camera::setViewport(int x, int y, int width, int height) {
    viewportX = x;
    viewportY = y;
    viewportWidth = width;
    viewportHeight = height;
    if (RenderEngine::instance->getCamera() == this){
        glViewport(x, y, width, height);
        glScissor(x, y, width, height);
    }
}
