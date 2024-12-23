#include "GameObjectCamera.h"
#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

GameObjectCamera::GameObjectCamera() {
    // Inicialización por defecto
    position = glm::vec3(0.0f, 2.0f, -5.0f);
    target = glm::vec3(0.0f, 0.0f, 1.0f);
}

void GameObjectCamera::OnPlay() {
    isActive = true;
    LockMouse();
    SDL_GetMouseState(&lastMouseX, &lastMouseY);
}

void GameObjectCamera::OnStop() {
    isActive = false;
    UnlockMouse();
}

void GameObjectCamera::LockMouse() {
    isMouseLocked = true;
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void GameObjectCamera::UnlockMouse() {
    isMouseLocked = false;
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void GameObjectCamera::ProcessInput() {
    if (!isActive) return;

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

    if (keystate[SDL_SCANCODE_W]) {
        position += forward * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_S]) {
        position -= forward * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_A]) {
        position -= right * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_D]) {
        position += right * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_SPACE]) {
        position.y += currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_LCTRL]) {
        position.y -= currentSpeed * deltaTime;
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

        // Actualizar el vector target basado en los nuevos ángulos
        target = position + front;
    }
}

void GameObjectCamera::Update() {
    if (!isActive) return;
    ProcessInput();
}