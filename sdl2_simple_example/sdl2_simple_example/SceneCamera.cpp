#include "SceneCamera.h"


void SceneCamera::MoveCamera(const Uint8* keystate) {
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
        cameraX -= forward.x * adjustedSpeed;
        cameraY -= forward.y * adjustedSpeed;
        cameraZ -= forward.z * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_S]) {
        cameraX += forward.x * adjustedSpeed;
        cameraY += forward.y * adjustedSpeed;
        cameraZ += forward.z * adjustedSpeed;
    }

    // Movimiento lateral
    if (keystate[SDL_SCANCODE_D]) {
        cameraX -= right.x * adjustedSpeed;
        cameraY -= right.y * adjustedSpeed;
        cameraZ -= right.z * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_A]) {
        cameraX += right.x * adjustedSpeed;
        cameraY += right.y * adjustedSpeed;
        cameraZ += right.z * adjustedSpeed;
    }
}

void SceneCamera::RotateCamera(int xrel, int yrel) {
    const float sensitivity = 0.3f;

    orbitAngleHorizontal += xrel * sensitivity;
    orbitAngleVertical += yrel * sensitivity;

    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    // Actualizar la posici�n de la c�mara basada en los �ngulos
    cameraX = targetX + orbitRadius * cos(verticalRad) * sin(horizontalRad);
    cameraY = targetY + orbitRadius * sin(verticalRad);
    cameraZ = targetZ + orbitRadius * cos(verticalRad) * cos(horizontalRad);
}

void SceneCamera::FocusOnObject() {
    // Cambia targetX, Y, Z seg�n el objeto que deseas centrar
    targetX = 0.0f;
    targetY = 0.0f;
    targetZ = 0.0f;
    orbitAngleHorizontal = 0.0f;
    orbitAngleVertical = -30.0f;
    orbitRadius = 10.0f;  // Ajusta el radio seg�n el tama�o del objeto
    RotateCamera(0, 0);
}
void SceneCamera::MoveCameraWithMouse(int xrel, int yrel) {
    float sensitivity = 0.05f;
    glm::vec3 up(0.0f, 1.0f, 0.0f);  // Eje vertical para mover en el eje Y
    glm::vec3 right(-1.0f, 0.0f, 0.0f);  // Eje vertical para mover en el eje X

    // C�lculo del desplazamiento en el eje horizontal y vertical
    glm::vec3 horizontalMovement = right * (-xrel * sensitivity);
    glm::vec3 verticalMovement = up * (yrel * sensitivity);

    // Actualizar la posici�n de la c�mara y el objetivo
    cameraX += horizontalMovement.x + verticalMovement.x;
    cameraY += horizontalMovement.y + verticalMovement.y;
    cameraZ += horizontalMovement.z + verticalMovement.z;

    targetX += horizontalMovement.x + verticalMovement.x;
    targetY += horizontalMovement.y + verticalMovement.y;
    targetZ += horizontalMovement.z + verticalMovement.z;
}

void ProcessInput(const SDL_Event& event, const Uint8* keystate) {
    switch (event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_f) {
            FocusOnObject();
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_RIGHT) {
            rotatingCamera = true;
        }
        if (event.button.button == SDL_BUTTON_LEFT && keystate[SDL_SCANCODE_LALT]) {
            rotatingCamera = true;
        }
        if (event.button.button == SDL_BUTTON_MIDDLE && keystate[SDL_SCANCODE_LALT]) {
            movingCameraWithMouse = true;
        }
        break;

    case SDL_MOUSEBUTTONUP:
        rotatingCamera = false;
        movingCameraWithMouse = false;
        break;

    case SDL_MOUSEMOTION:
        if (rotatingCamera) {
            RotateCamera(event.motion.xrel, event.motion.yrel);
        }
        if (movingCameraWithMouse) {
            MoveCameraWithMouse(event.motion.xrel, event.motion.yrel);
        }
        break;

    case SDL_MOUSEWHEEL:
        orbitRadius += (event.wheel.y > 0) ? -0.5f : 0.5f;
        orbitRadius = glm::clamp(orbitRadius, 2.0f, 50.0f);
        break;
    }
    MoveCamera(keystate);
}
