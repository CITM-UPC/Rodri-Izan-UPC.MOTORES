#include "Camera.h"
#include <cmath>

Camera::Camera()
    : position(0.0f, -10.0f, 10.0f),
    target(0.0f, 0.0f, 0.0f),
    upVector(0.0f, 1.0f, 0.0f),
    fov(45.0f),
    nearPlane(0.1f),
    farPlane(100.0f),
    aspectRatio(16.0f / 9.0f),
    cameraSpeed(0.05f),
    orbitRadius(10.0f),
    sensitivity(0.3f),
    orbitAngleHorizontal(0.0f),
    orbitAngleVertical(-30.0f)
{
}

void Camera::SetPosition(const glm::vec3& pos) {
    position = pos;
}

void Camera::SetTarget(const glm::vec3& tgt) {
    target = tgt;
}

void Camera::SetOrbitRadius(float radius) {
    orbitRadius = glm::clamp(radius, 2.0f, 30.0f);
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(position, target, upVector);
}

void Camera::UpdateProjection(float width, float height) {
    aspectRatio = width / height;
}

void Camera::Zoom(float amount) {
    if (this != nullptr) {
        orbitRadius = glm::clamp(orbitRadius + amount, 2.0f, 30.0f);
        UpdateCameraPosition();
    }
}

void Camera::UpdateCameraPosition() {
    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    position.x = target.x + orbitRadius * std::cos(verticalRad) * std::sin(horizontalRad);
    position.y = target.y + orbitRadius * std::sin(verticalRad);
    position.z = target.z + orbitRadius * std::cos(verticalRad) * std::cos(horizontalRad);
}