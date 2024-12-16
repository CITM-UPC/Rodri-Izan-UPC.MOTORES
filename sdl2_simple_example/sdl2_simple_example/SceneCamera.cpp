#include "SceneCamera.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdexcept>

SceneCamera::SceneCamera() : Camera() {}

void SceneCamera::Move(const Uint8* keystate) {
    float adjustedSpeed = (keystate[SDL_SCANCODE_LSHIFT] ? cameraSpeed * 2.0f : cameraSpeed);
    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    glm::vec3 forward(
        std::cos(verticalRad) * std::sin(horizontalRad),
        std::sin(verticalRad),
        std::cos(verticalRad) * std::cos(horizontalRad)
    );

    glm::vec3 right = glm::normalize(glm::cross(forward, upVector));

    // Movimiento adelante y atrás
    if (keystate[SDL_SCANCODE_W]) {
        position -= forward * adjustedSpeed;
        target -= forward * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_S]) {
        position += forward * adjustedSpeed;
        target += forward * adjustedSpeed;
    }

    // Movimiento lateral
    if (keystate[SDL_SCANCODE_D]) {
        position -= right * adjustedSpeed;
        target -= right * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_A]) {
        position += right * adjustedSpeed;
        target += right * adjustedSpeed;
    }
}

void SceneCamera::Rotate(int xrel, int yrel) {
    orbitAngleHorizontal += xrel * sensitivity;
    orbitAngleVertical += yrel * sensitivity;

    // Limitar la rotación vertical para evitar volcar la cámara
    orbitAngleVertical = glm::clamp(orbitAngleVertical, -89.0f, 89.0f);

    UpdateCameraPosition();
}

void SceneCamera::FocusOnObject() {
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    orbitAngleHorizontal = 0.0f;
    orbitAngleVertical = -30.0f;
    orbitRadius = 10.0f;
    UpdateCameraPosition();
}

void SceneCamera::MoveCameraWithMouse(int xrel, int yrel) {
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(target - position, up));

    glm::vec3 horizontalMovement = right * (-xrel * sensitivity);
    glm::vec3 verticalMovement = up * (yrel * sensitivity);

    position += horizontalMovement + verticalMovement;
    target += horizontalMovement + verticalMovement;
}

void SceneCamera::FocusOnObject() {
    // Cambia targetX, Y, Z seg?n el objeto que deseas centrar
    target.x = 0.0f;
    target.y = 0.0f;
    target.z = 0.0f;
    orbitAngleHorizontal = 0.0f;
    orbitAngleVertical = -30.0f;
    orbitRadius = 10.0f;  // Ajusta el radio seg?n el tama?o del objeto
    Rotate(0, 0);
}