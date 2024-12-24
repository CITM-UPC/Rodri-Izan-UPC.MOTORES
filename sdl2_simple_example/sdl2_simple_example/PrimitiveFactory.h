#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "GameObjectManager.h"
#include "GameObjectCamera.h"

class PrimitiveFactory {
public:
    enum class PrimitiveType {
        Cube,
        Sphere,
        Cylinder,
        Plane,
        Cone
    };

    static RenderableGameObject* CreatePrimitive(PrimitiveType type);
    static Camera* CreateCamera();

private:
    static std::vector<glm::vec3> GenerateCubeVertices();
    static std::vector<glm::vec3> GenerateSphereVertices(float radius = 1.0f, int segments = 32);
    static std::vector<glm::vec3> GenerateCylinderVertices(float radius = 1.0f, float height = 2.0f, int segments = 32);
    static std::vector<glm::vec3> GeneratePlaneVertices();
    static std::vector<glm::vec3> GenerateConeVertices(float radius = 1.0f, float height = 2.0f, int segments = 32);
};
