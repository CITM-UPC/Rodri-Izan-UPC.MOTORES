#include "GameObjectCamera.h"
#include <glm/gtc/matrix_transform.hpp>

void GameObjectCamera::ProcessInput() {
    if (!Camera::IsActive()) return;

    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    float deltaTime = 1.0f / 60.0f; // Idealmente esto debería venir del game loop

    // Calcular la dirección frontal basada en la rotación actual
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    glm::vec3 forward = glm::normalize(front);

    // Calcular el vector derecho
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Movimiento WASD
    float currentSpeed = movementSpeed;
    if (keystate[SDL_SCANCODE_LSHIFT]) {
        currentSpeed *= 2.0f;
    }

    glm::vec3 movement(0.0f);

    if (keystate[SDL_SCANCODE_W]) {
        movement += forward * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_S]) {
        movement -= forward * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_A]) {
        movement -= right * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_D]) {
        movement += right * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_SPACE]) {
        movement.y += currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_LCTRL]) {
        movement.y -= currentSpeed * deltaTime;
    }

    if (glm::length(movement) > 0) {
        SetLocalPosition(GetLocalPosition() + movement);
    }

    // Procesar movimiento del ratón si está bloqueado
    if (isMouseLocked) {
        int mouseX, mouseY;
        SDL_GetRelativeMouseState(&mouseX, &mouseY);

        yaw += mouseX * mouseSensitivity;
        pitch -= mouseY * mouseSensitivity;

        // Limitar el pitch para evitar que la cámara se de la vuelta
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // Actualizar la rotación del GameObject
        SetLocalRotation(glm::vec3(pitch, yaw, 0.0f));
    }
}

void GameObjectCamera::LockMouse() {
    isMouseLocked = true;
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void GameObjectCamera::UnlockMouse() {
    isMouseLocked = false;
    SDL_SetRelativeMouseMode(SDL_FALSE);
}