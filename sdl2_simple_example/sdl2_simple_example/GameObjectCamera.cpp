#include "GameObjectCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

GameObjectCamera::GameObjectCamera(const std::string& name)
    : RenderableGameObject(name), Camera() {
    SetPosition(glm::vec3(0.0f, 2.0f, -5.0f));
    SetRotation(glm::vec3(0.0f));
    yaw = -90.0f;
    pitch = 0.0f;
    movementSpeed = 2.5f;
    mouseSensitivity = 0.1f;
    Camera::SetActive(false);
    isMouseLocked = false;
}


void GameObjectCamera::OnPlay() {
    Camera::SetActive(true);
    LockMouse();  // Bloquea el ratón
    SDL_GetMouseState(&lastMouseX, &lastMouseY);
}

void GameObjectCamera::OnStop() {
    Camera::SetActive(false);
    UnlockMouse();  // Desbloquea el ratón
}

void GameObjectCamera::LockMouse() {
    isMouseLocked = true;
    SDL_SetRelativeMouseMode(SDL_TRUE); // Habilitar el modo relativo
    SDL_ShowCursor(SDL_DISABLE);        // Deshabilitar el cursor visual
}

void GameObjectCamera::UnlockMouse() {
    isMouseLocked = false;
    SDL_SetRelativeMouseMode(SDL_FALSE); // Deshabilitar el modo relativo
    SDL_ShowCursor(SDL_ENABLE);          // Habilitar el cursor visual
}

void GameObjectCamera::ProcessInput() {
    if (!Camera::IsActive()) return;

    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    float deltaTime = 1.0f / 60.0f;

    if (keystate[SDL_SCANCODE_F5]) {
        if (!isMouseLocked) {
            OnPlay();  // Activa la cámara y bloquea el ratón
        }
        else {
            OnStop();  // Detiene la cámara y desbloquea el ratón
        }
    }

    // Calcular la dirección frontal basada en la rotación actual
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    glm::vec3 forward = glm::normalize(front);

    // Calcular el vector derecho
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    float currentSpeed = movementSpeed;
    if (keystate[SDL_SCANCODE_LSHIFT]) {
        currentSpeed *= 2.0f;
    }

    glm::vec3 newPosition = GetPosition();

    if (keystate[SDL_SCANCODE_W]) {
        newPosition += forward * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_S]) {
        newPosition -= forward * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_A]) {
        newPosition -= right * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_D]) {
        newPosition += right * currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_SPACE]) {
        newPosition.y += currentSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_LCTRL]) {
        newPosition.y -= currentSpeed * deltaTime;
    }

    SetPosition(newPosition);

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
        Camera::SetTarget(GetPosition() + front);
    }
}

void GameObjectCamera::Update() {
    if (!Camera::IsActive()) return;
    ProcessInput();
}

glm::mat4 GameObjectCamera::GetViewMatrix() const {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    return glm::lookAt(
        GetPosition(),
        GetPosition() + glm::normalize(front),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

glm::mat4 GameObjectCamera::GetProjectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(GetFOV()), aspect, GetNearPlane(), GetFarPlane());
}



