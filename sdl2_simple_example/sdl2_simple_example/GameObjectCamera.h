#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "Importer.h"
#include <SDL2/SDL_events.h>

class GameObjectCamera : public RenderableGameObject, public Camera {
private:
    float mouseSensitivity = 0.2f;
    float movementSpeed = 5.0f;
    bool isMouseLocked = false;
    int lastMouseX = 0;
    int lastMouseY = 0;
    float yaw = 0.0f;
    float pitch = 0.0f;

public:
    GameObjectCamera(const std::string& name = "Camera");
    ~GameObjectCamera() override = default;

    void ProcessInput();
    void Update() override;
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspect) const;

    void OnPlay() override;
    void OnStop() override;

    void SetMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void SetMovementSpeed(float speed) { movementSpeed = speed; }

    void LockMouse();
    void UnlockMouse();

    // Resolver ambigüedades de herencia múltiple
    using RenderableGameObject::SetPosition;
    using RenderableGameObject::GetPosition;
    using Camera::SetActive;
    using Camera::IsActive;

};