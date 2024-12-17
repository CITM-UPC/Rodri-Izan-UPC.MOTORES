#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include "Camera.h"

class SceneCamera : public Camera {
public:
    SceneCamera();

    void UpdateCameraPosition();

    // Implementaciones específicas de SceneCamera
    void Move(const Uint8* keystate) override;
    void Rotate(int xrel, int yrel) override;
    void FocusOnObject() override;

    // Métodos específicos de SceneCamera
    void MoveCameraWithMouse(int xrel, int yrel);

    // Getters for camera properties
    glm::vec3 GetCameraPosition() const { return glm::vec3(position.x, position.y, position.z); }
    glm::vec3 GetTargetPosition() const { return glm::vec3(target.x, target.y, target.z); }

    bool rotatingCamera = false;
    bool movingCameraWithMouse = false;

    float cameraSpeed = 5.f;        // Velocidad de movimiento
    float sensitivity = 5.f;         // Sensibilidad de rotación
    float orbitRadius = 10.0f;        // Radio orbital inicial
    float orbitAngleHorizontal = 0.0f;
    float orbitAngleVertical = -30.0f;
};

#endif // SCENE_CAMERA_H