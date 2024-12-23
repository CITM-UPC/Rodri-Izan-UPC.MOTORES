#include "PrimitiveFactory.h"
#include "Importer.h"
#include <iostream>
#include "GameObjectCamera.h"


RenderableGameObject* PrimitiveFactory::CreatePrimitive(PrimitiveType type) {
    RenderableGameObject* obj = nullptr;
    std::string name;
    std::string meshFile;

    // Definir el nombre y archivo de la malla según el tipo
    switch (type) {
    case PrimitiveType::Cube:
        name = "Cube";
        meshFile = "Assets/Primitives/Cube.fbx";
        break;
    case PrimitiveType::Sphere:
        name = "Sphere";
        meshFile = "Assets/Primitives/Sphere.fbx";
        break;
    case PrimitiveType::Cylinder:
        name = "Cylinder";
        meshFile = "Assets/Primitives/Cylinder.fbx";
        break;
    case PrimitiveType::Plane:
        name = "Plane";
        meshFile = "Assets/Primitives/Plane.fbx";
        break;
    case PrimitiveType::Cone:
        name = "Cone";
        meshFile = "Assets/Primitives/Cone.fbx";
        break;
    default:
        return nullptr; // Tipo desconocido
    }

    // Crear un objeto RenderableGameObject con el nombre asignado
    auto& manager = GameObjectManager::GetInstance();
    obj = manager.CreateGameObject<RenderableGameObject>(name);

    // Utilizar Importer para cargar la malla
    Importer importer;
    if (importer.ImportFBX(meshFile)) {
        const auto* model = importer.GetModel(name); // Obtener el modelo cargado
        if (model && !model->meshes.empty()) {
            // Asignar la primera malla del modelo al objeto
            obj->SetMeshIndex(0); // Índice 0 como ejemplo
            obj->SetTextureID(0); // Si el modelo tiene textura, puedes asignarla aquí
        }
    }
    else {
        std::cerr << "Error: Could not load primitive mesh from file: " << meshFile << std::endl;
    }

    // Configurar transformaciones iniciales del objeto
    obj->SetPosition(glm::vec3(0.0f));
    obj->SetRotation(glm::vec3(0.0f));
    obj->SetScale(glm::vec3(1.0f));

    return obj;
}
std::vector<glm::vec3> PrimitiveFactory::GenerateCubeVertices() {
    return {
        glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f,  1.0f, -1.0f),
        glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(1.0f,  1.0f,  1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f, -1.0f,  1.0f),
        glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f, -1.0f,  1.0f),
        glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(1.0f, -1.0f,  1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f)
    };
}

std::vector<glm::vec3> PrimitiveFactory::GenerateSphereVertices(float radius, int segments) {
    std::vector<glm::vec3> vertices;

    for (int lat = 0; lat <= segments; ++lat) {
        float theta = lat * glm::pi<float>() / segments;
        float sinTheta = glm::sin(theta);
        float cosTheta = glm::cos(theta);

        for (int lon = 0; lon <= segments; ++lon) {
            float phi = lon * 2.0f * glm::pi<float>() / segments;
            float sinPhi = glm::sin(phi);
            float cosPhi = glm::cos(phi);

            float x = radius * cosPhi * sinTheta;
            float y = radius * cosTheta;
            float z = radius * sinPhi * sinTheta;

            vertices.push_back(glm::vec3(x, y, z));
        }
    }

    return vertices;
}


std::vector<glm::vec3> PrimitiveFactory::GenerateCylinderVertices(float radius, float height, int segments) {
    std::vector<glm::vec3> vertices;

    // Generar vértices para la parte superior e inferior
    for (int i = 0; i < segments; ++i) {
        float angle = i * 2.0f * glm::pi<float>() / segments;
        float x = radius * glm::cos(angle);
        float z = radius * glm::sin(angle);

        // Parte superior
        vertices.push_back(glm::vec3(x, height / 2.0f, z));

        // Parte inferior
        vertices.push_back(glm::vec3(x, -height / 2.0f, z));
    }

    return vertices;
}


std::vector<glm::vec3> PrimitiveFactory::GeneratePlaneVertices() {
    return {
        glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f,  1.0f),
        glm::vec3(1.0f, 0.0f,  1.0f), glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(-1.0f, 0.0f, -1.0f)
    };
}


std::vector<glm::vec3> PrimitiveFactory::GenerateConeVertices(float radius, float height, int segments) {
    std::vector<glm::vec3> vertices;

    // Generar vértices de la base
    for (int i = 0; i < segments; ++i) {
        float angle = i * 2.0f * glm::pi<float>() / segments;
        float x = radius * glm::cos(angle);
        float z = radius * glm::sin(angle);

        // Base
        vertices.push_back(glm::vec3(x, -height / 2.0f, z));
    }

    // Vértice superior del cono
    vertices.push_back(glm::vec3(0.0f, height / 2.0f, 0.0f));

    return vertices;
}

Camera* PrimitiveFactory::CreateCamera() {
    // Crear un objeto GameObjectCamera
    GameObjectCamera* camera = new GameObjectCamera();

    // Configurar su posición y rotación si es necesario
    camera->SetPosition(glm::vec3(0.0f, 2.0f, -5.0f)); // Posición inicial
    camera->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f)); // Rotación inicial

    return camera; // Retornamos un puntero a Camera*
}

