#pragma once
#include "Camera.h"
#include <SDL2/SDL_events.h>

class SceneCamera : public Camera {
private:
    bool rotatingCamera = false;
    bool movingCamera = false;
    bool movingCameraWithMouse = false;

public:
    SceneCamera() = default;
    ~SceneCamera() override = default;

    void ProcessKeyboardInput(const Uint8* keystate);
    void ProcessMouseMotion(int xrel, int yrel);
    void ProcessMouseWheel(int y);
    void FocusOnObject();
    void MoveCameraWithMouse(int xrel, int yrel);
    void RotateCamera(int xrel, int yrel);

    // Input state handlers
    void SetRotatingCamera(bool rotating) { rotatingCamera = rotating; }
    void SetMovingCameraWithMouse(bool moving) { movingCameraWithMouse = moving; }
    bool IsRotatingCamera() const { return rotatingCamera; }
    bool IsMovingCameraWithMouse() const { return movingCameraWithMouse; }
};

