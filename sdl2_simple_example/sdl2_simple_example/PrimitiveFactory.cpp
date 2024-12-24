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

Camera* PrimitiveFactory::CreateCamera() {
    // Crear un objeto GameObjectCamera
    GameObjectCamera* camera = new GameObjectCamera();

    // Configurar su posición y rotación si es necesario
    camera->SetPosition(glm::vec3(0.0f, 2.0f, -5.0f)); // Posición inicial
    camera->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f)); // Rotación inicial

    return camera; // Retornamos un puntero a Camera*
}

