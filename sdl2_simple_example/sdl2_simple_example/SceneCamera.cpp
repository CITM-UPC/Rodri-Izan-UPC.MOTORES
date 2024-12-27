#include "SceneCamera.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>

void SceneCamera::ProcessKeyboardInput(const Uint8* keystate) {
    float adjustedSpeed = (keystate[SDL_SCANCODE_LSHIFT] ? moveSpeed * 2.0f : moveSpeed);
    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    glm::vec3 forward(
        cos(verticalRad) * sin(horizontalRad),
        sin(verticalRad),
        cos(verticalRad) * cos(horizontalRad)
    );

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    if (keystate[SDL_SCANCODE_W]) {
        position -= forward * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_S]) {
        position += forward * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_D]) {
        position -= right * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_A]) {
        position += right * adjustedSpeed;
    }
}

void SceneCamera::RotateCamera(int xrel, int yrel) {
    const float sensitivity = 0.3f;

    orbitAngleHorizontal += xrel * sensitivity;
    orbitAngleVertical += yrel * sensitivity;

    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    position.x = target.x + orbitRadius * cos(verticalRad) * sin(horizontalRad);
    position.y = target.y + orbitRadius * sin(verticalRad);
    position.z = target.z + orbitRadius * cos(verticalRad) * cos(horizontalRad);
}

void SceneCamera::MoveCameraWithMouse(int xrel, int yrel) {
    float sensitivity = 0.05f;
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right(-1.0f, 0.0f, 0.0f);

    glm::vec3 horizontalMovement = right * (-xrel * sensitivity);
    glm::vec3 verticalMovement = up * (yrel * sensitivity);

    position += horizontalMovement + verticalMovement;
    target += horizontalMovement + verticalMovement;
}

void SceneCamera::ProcessMouseMotion(int xrel, int yrel) {
    if (rotatingCamera) {
        RotateCamera(xrel, yrel);
    }
    if (movingCameraWithMouse) {
        MoveCameraWithMouse(xrel, yrel);
    }
}

void SceneCamera::ProcessMouseWheel(int y) {
    if (y > 0) {
        orbitRadius = std::max(orbitRadius - 1.0f, 2.0f);
    }
    else if (y < 0) {
        orbitRadius = std::min(orbitRadius + 1.0f, 30.0f);
    }
    RotateCamera(0, 0);
}

void SceneCamera::FocusOnObject() {
    target = glm::vec3(0.0f);
    orbitAngleHorizontal = 0.0f;
    orbitAngleVertical = -30.0f;
    orbitRadius = 10.0f;
    RotateCamera(0, 0);
}

