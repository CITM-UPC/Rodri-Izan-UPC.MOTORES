#pragma once
#include "Camera.h"
#include "GameObject.h"
#include <SDL2/SDL_events.h>

class GameObjectCamera : public GameObject, public Camera {
private:
    float mouseSensitivity = 0.2f;
    float movementSpeed = 5.0f;
    // Estado de entrada
    bool isMouseLocked = false;
    int lastMouseX = 0;
    int lastMouseY = 0;
    float yaw = -90.0f;
    float pitch = 0.0f;

public:
    GameObjectCamera(const std::string& name = "Camera")
        : GameObject(name), Camera() {
        Camera::SetPosition(glm::vec3(0.0f, 2.0f, -5.0f));
        Camera::SetTarget(glm::vec3(0.0f, 0.0f, 1.0f));
        movementSpeed = 2.5f;
        mouseSensitivity = 0.1f;
        Camera::SetActive(false);
        isMouseLocked = false;
    }

    ~GameObjectCamera() override = default;

    void ProcessInput();

    void Update() override {
        if (!Camera::IsActive()) return;
        ProcessInput();
    }

    // Sobreescribimos los métodos de transformación para mantener sincronizada la cámara
    void SetLocalPosition(const glm::vec3& pos) {
        GameObject::SetLocalPosition(pos);
        Camera::SetPosition(GetGlobalPosition());
        UpdateTarget();
    }

    void SetLocalRotation(const glm::vec3& rot) {
        GameObject::SetLocalRotation(rot);
        yaw = rot.y;
        pitch = rot.x;
        UpdateTarget();
    }

    // Métodos de la cámara
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(Camera::GetPosition(), Camera::GetTarget(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 GetProjectionMatrix(float fov, float aspect, float near, float far) const {
        return glm::perspective(glm::radians(fov), aspect, near, far);
    }

    // Resolvemos la ambigüedad especificando qué versión usar
    using Camera::SetPosition;  // Usamos la versión de Camera para estos métodos
    using Camera::GetPosition;
    using Camera::SetTarget;
    using Camera::GetTarget;
    using Camera::SetActive;
    using Camera::IsActive;

    // Sobrescribimos los métodos virtuales de Camera
    void OnPlay() override {
        Camera::SetActive(true);
        LockMouse();
        SDL_GetMouseState(&lastMouseX, &lastMouseY);
    }

    void OnStop() override {
        Camera::SetActive(false);
        UnlockMouse();
    }

    void SetMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void SetMovementSpeed(float speed) { movementSpeed = speed; }

    // Métodos de control
    void LockMouse();
    void UnlockMouse();

private:
    void UpdateTarget() {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Camera::SetTarget(Camera::GetPosition() + glm::normalize(front));
    }
};