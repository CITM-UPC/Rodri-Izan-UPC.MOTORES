#pragma once
#include "GameObject.h"
#include "Camera.h"
#include <SDL2/SDL_events.h>

class GameObjectCamera : public GameObject, public Camera {
private:
    float mouseSensitivity = 0.2f;
    float movementSpeed = 5.0f;
    // Estado de entrada
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

    // Métodos de control
    void LockMouse();
    void UnlockMouse();

    // Override de los métodos que podrían causar conflicto
    using GameObject::SetPosition;  // Usamos el SetPosition de GameObject
    using GameObject::GetPosition;  // Usamos el GetPosition de GameObject
    using Camera::SetActive;        // Usamos el SetActive de Camera
    using Camera::IsActive;         // Usamos el IsActive de Camera
};