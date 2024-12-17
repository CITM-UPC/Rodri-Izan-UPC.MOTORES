#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>

class Camera {
protected:
    // Posición de la cámara
    glm::vec3 position;

    // Punto al que mira la cámara
    glm::vec3 target;

    // Vector "arriba" para orientar la cámara
    glm::vec3 upVector;

    // Parámetros de la cámara
    float fov;
    float nearPlane;
    float farPlane;
    float aspectRatio;

    // Parámetros de movimiento
    float cameraSpeed;

    float sensitivity;

    // Ángulos de rotación
    float orbitAngleHorizontal;
    float orbitAngleVertical;

public:
    Camera();
    virtual ~Camera() = default;
    float orbitRadius;
    // Getters y setters básicos
    glm::vec3 GetPosition() const { return position; }
    void SetPosition(const glm::vec3& pos);

    glm::vec3 GetTarget() const { return target; }
    void SetTarget(const glm::vec3& tgt);

    float GetOrbitRadius() const { return orbitRadius; }
    void SetOrbitRadius(float radius);

    // Métodos virtuales para permitir comportamientos específicos en las subclases
    virtual void Move(const Uint8* keystate) = 0;
    virtual void Rotate(int xrel, int yrel) = 0;
    virtual void FocusOnObject() = 0;

    // Método para obtener la matriz de vista
    glm::mat4 GetViewMatrix() const;

    // Método para actualizar la proyección
    void UpdateProjection(float width, float height);

    void Zoom(float amount);

protected:
    // Método interno para calcular la posición de la cámara basada en ángulos
    void UpdateCameraPosition();
};

#endif // CAMERA_H