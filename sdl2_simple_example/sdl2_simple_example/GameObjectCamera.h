#pragma once
#include "Camera.h"
#include <SDL2/SDL_events.h>

class GameObjectCamera : public Camera {
private:
    glm::vec3 rotation = glm::vec3(0.0f);
    float mouseSensitivity = 0.2f;
    float movementSpeed = 5.0f;

    // Estado de entrada
    bool isMouseLocked = false;
    int lastMouseX = 0;
    int lastMouseY = 0;
    float yaw = 0.0f;
    float pitch = 0.0f;

public:
    GameObjectCamera();
    ~GameObjectCamera() override = default;

    void ProcessInput();
    void Update() override;
    void OnPlay() override;
    void OnStop() override;

    void SetRotation(const glm::vec3& rot) { rotation = rot; }
    glm::vec3 GetRotation() const { return rotation; }

    void SetMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void SetMovementSpeed(float speed) { movementSpeed = speed; }

    // Métodos de control
    void LockMouse();
    void UnlockMouse();
};