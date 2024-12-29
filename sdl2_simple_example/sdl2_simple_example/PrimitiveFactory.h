#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "GameObjectManager.h"
#include "GameObjectCamera.h"
#include "Importer.h"

class PrimitiveFactory {
public:
    enum class PrimitiveType {
        Cube,
        Sphere,
        Cylinder,
        Plane,
        Cone,
        Camera
    };

    static RenderableGameObject* CreatePrimitive(PrimitiveType type, Importer* importer);
    static Camera* CreateCamera(Importer* importer);

private:
    static const std::string PRIMITIVES_PATH;
};
