#pragma once
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera
{
public:
    // Camera properties
    GLfloat cameraX = 0.0f;
    GLfloat cameraY = 5.0f;
    GLfloat cameraZ = 10.0f;
    GLfloat cameraSpeed = 0.1f;

    GLfloat cameraAngleX = 0.0f;
    GLfloat cameraAngleY = 0.0f;
    GLfloat cameraAngleZ = 0.0f;


    GLfloat targetX = 0.0f;
    GLfloat targetY = 0.0f;
    GLfloat targetZ = 0.0f;
    GLfloat orbitRadius = 10.0f;
    GLfloat orbitAngleHorizontal = 0.0f;
    GLfloat orbitAngleVertical = 30.0f;


    // Getters for camera properties
    glm::vec3 GetCameraPosition() const { return glm::vec3(cameraX, cameraY, cameraZ); }
    glm::vec3 GetTargetPosition() const { return glm::vec3(targetX, targetY, targetZ); }

    GLfloat GetOrbitRadius() const { return orbitRadius; }
    void SetOrbitRadius(GLfloat radius) { orbitRadius = radius; }
};

