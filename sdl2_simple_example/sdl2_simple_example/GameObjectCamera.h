#ifndef GAME_OBJECT_CAMERA_H
#define GAME_OBJECT_CAMERA_H

#include "Camera.h"

class GameObjectCamera : public Camera {
private:
    // Propiedades específicas de GameObjectCamera
    bool isPerspective;

public:
    GameObjectCamera();

    // Implementaciones específicas de GameObjectCamera
    void Move(const Uint8* keystate) override;
    void Rotate(int xrel, int yrel) override;
    void FocusOnObject() override;

    // Métodos específicos de GameObjectCamera
    void SetPerspective(bool perspective);
    bool IsPerspective() const;
};

#endif // GAME_OBJECT_CAMERA_H
