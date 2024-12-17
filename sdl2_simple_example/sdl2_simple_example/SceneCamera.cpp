#include "SceneCamera.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdexcept>

SceneCamera::SceneCamera() : Camera() {}

void SceneCamera::UpdateCameraPosition() {
    // Convertir ángulos a radianes
    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    // Calcular la nueva posición usando coordenadas esféricas
    position.x = target.x + orbitRadius * std::cos(verticalRad) * std::sin(horizontalRad);
    position.y = target.y + orbitRadius * std::sin(verticalRad);
    position.z = target.z + orbitRadius * std::cos(verticalRad) * std::cos(horizontalRad);
}

void SceneCamera::Move(const Uint8* keystate) {
    float adjustedSpeed = (keystate[SDL_SCANCODE_LSHIFT] ? cameraSpeed * 2.0f : cameraSpeed);
    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    glm::vec3 forward(
        cos(verticalRad) * sin(horizontalRad),
        sin(verticalRad),
        cos(verticalRad) * cos(horizontalRad)
    );

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Movimiento adelante y atr�s
    if (keystate[SDL_SCANCODE_W]) {
        position.x -= forward.x * adjustedSpeed;
        position.y -= forward.y * adjustedSpeed;
        position.z -= forward.z * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_S]) {
        position.x += forward.x * adjustedSpeed;
        position.y += forward.y * adjustedSpeed;
        position.z += forward.z * adjustedSpeed;
    }

    // Movimiento lateral
    if (keystate[SDL_SCANCODE_D]) {
        position.x -= right.x * adjustedSpeed;
        position.y -= right.y * adjustedSpeed;
        position.z -= right.z * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_A]) {
        position.x += right.x * adjustedSpeed;
        position.y += right.y * adjustedSpeed;
        position.z += right.z * adjustedSpeed;
    }

    // Actualizar posición de la cámara manteniendo la relación orbital
    UpdateCameraPosition();
}

void SceneCamera::Rotate(int xrel, int yrel) {
    orbitAngleHorizontal += xrel * sensitivity;
    orbitAngleVertical += yrel * sensitivity;

    // Limitar la rotación vertical para evitar volcar la cámara
    orbitAngleVertical = glm::clamp(orbitAngleVertical, -89.0f, 89.0f);

    UpdateCameraPosition();
}

void SceneCamera::MoveCameraWithMouse(int xrel, int yrel) {
    glm::vec3 right = glm::normalize(glm::cross(target - position, upVector));
    glm::vec3 up = glm::normalize(glm::cross(right, target - position));

    glm::vec3 movement = right * (-xrel * sensitivity) + up * (yrel * sensitivity);

    target += movement;
    UpdateCameraPosition();
}

void SceneCamera::FocusOnObject() {
    // Ajusta según tus necesidades específicas
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    orbitAngleHorizontal = 0.0f;
    orbitAngleVertical = -30.0f;
    orbitRadius = 10.0f;

    UpdateCameraPosition();
}