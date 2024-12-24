#pragma once
#include <glm/glm.hpp>

class Camera {
protected:
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    glm::vec3 position = glm::vec3(0.0f, 5.0f, 10.0f);
    glm::vec3 target = glm::vec3(0.0f);
    float moveSpeed = 0.1f;

    // Propiedades de órbita
    float orbitRadius = 10.0f;
    float orbitAngleHorizontal = 0.0f;
    float orbitAngleVertical = 30.0f;

    bool isActive = false;

public:
    Camera() = default;
    virtual ~Camera() = default;

    // Getters
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetTarget() const { return target; }
    float GetFOV() const { return fov; }
    float GetNearPlane() const { return nearPlane; }
    float GetFarPlane() const { return farPlane; }
    float GetOrbitRadius() const { return orbitRadius; }
    bool IsActive() const { return isActive; }

    // Setters
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetTarget(const glm::vec3& tar) { target = tar; }
    void SetFOV(float newFov) { fov = newFov; }
    void SetNearPlane(float near) { nearPlane = near; }
    void SetFarPlane(float far) { farPlane = far; }
    void SetMoveSpeed(float speed) { moveSpeed = speed; }
    void SetActive(bool active) { isActive = active; }

    // Métodos virtuales que pueden ser sobrescritos
    virtual void Update() {}
    virtual void OnPlay() {} // Nuevo método para cuando se inicia el modo juego
    virtual void OnStop() {} // Nuevo método para cuando se detiene el modo juego
};