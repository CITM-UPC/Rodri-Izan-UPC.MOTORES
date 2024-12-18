#pragma once
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Camera.h"

class IEventProcessor {
public:
    virtual void ProcessInput(const SDL_Event& event) = 0;
};

class SceneCamera: public Camera
{
public:

    bool rotatingCamera = false;
    bool movingCamera = false;
    bool movingCameraWithMouse = false;


    // Private camera methods
    void MoveCamera(const Uint8* keystate);
    void MoveCameraWithMouse(int xrel, int yrel);

    void RotateCamera(int xrel, int yrel);
    void FocusOnObject();

    SceneCamera() = default;
};

