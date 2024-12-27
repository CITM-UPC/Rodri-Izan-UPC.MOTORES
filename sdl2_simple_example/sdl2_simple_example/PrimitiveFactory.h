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
        Cone
    };

    static RenderableGameObject* CreatePrimitive(PrimitiveType type, Importer* importer);
    static Camera* CreateCamera();
private:
    static const std::string PRIMITIVES_PATH;
};
