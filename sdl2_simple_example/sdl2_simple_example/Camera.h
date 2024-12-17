#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>

class Camera {
protected:
    // Posici�n de la c�mara
    glm::vec3 position;

    // Punto al que mira la c�mara
    glm::vec3 target;

    // Vector "arriba" para orientar la c�mara
    glm::vec3 upVector;

    // Par�metros de la c�mara
    float fov;
    float nearPlane;
    float farPlane;
    float aspectRatio;

    // Par�metros de movimiento
    float cameraSpeed;

    float sensitivity;

    // �ngulos de rotaci�n
    float orbitAngleHorizontal;
    float orbitAngleVertical;

public:
    Camera();
    virtual ~Camera() = default;
    float orbitRadius;
    // Getters y setters b�sicos
    glm::vec3 GetPosition() const { return position; }
    void SetPosition(const glm::vec3& pos);

    glm::vec3 GetTarget() const { return target; }
    void SetTarget(const glm::vec3& tgt);

    float GetOrbitRadius() const { return orbitRadius; }
    void SetOrbitRadius(float radius);

    // M�todos virtuales para permitir comportamientos espec�ficos en las subclases
    virtual void Move(const Uint8* keystate) = 0;
    virtual void Rotate(int xrel, int yrel) = 0;
    virtual void FocusOnObject() = 0;

    // M�todo para obtener la matriz de vista
    glm::mat4 GetViewMatrix() const;

    // M�todo para actualizar la proyecci�n
    void UpdateProjection(float width, float height);

    void Zoom(float amount);

protected:
    // M�todo interno para calcular la posici�n de la c�mara basada en �ngulos
    void UpdateCameraPosition();
};

#endif // CAMERA_H