#include "PrimitiveFactory.h"
#include "Importer.h"
#include <iostream>
#include "GameObjectCamera.h"


const std::string PrimitiveFactory::PRIMITIVES_PATH = "Assets/Primitives/";

RenderableGameObject* PrimitiveFactory::CreatePrimitive(PrimitiveType type, Importer* importer) {
    if (!importer) return nullptr;

    RenderableGameObject* obj = nullptr;
    std::string name;
    std::string meshFile;

    switch (type) {
    case PrimitiveType::Cube:
        name = "Cube";
        meshFile = PRIMITIVES_PATH + "Cube.fbx";
        break;
    case PrimitiveType::Sphere:
        name = "Sphere";
        meshFile = PRIMITIVES_PATH + "Sphere.fbx";
        break;
    case PrimitiveType::Cone:
        name = "Cone";
        meshFile = PRIMITIVES_PATH + "Cone.fbx";
        break;
    case PrimitiveType::Plane:
        name = "Plane";
        meshFile = PRIMITIVES_PATH + "Plane.fbx";
        break;
    case PrimitiveType::Cylinder:
        name = "Cylinder";
        meshFile = PRIMITIVES_PATH + "Cylinder.fbx";
        break;


    }

    // Importar el modelo
    if (importer->ImportFBX(meshFile.c_str())) {
        auto& manager = GameObjectManager::GetInstance();
        const std::string modelName = importer->GetModelName(meshFile.c_str());
        const auto* model = importer->GetModel(modelName);

        if (model) {
            obj = manager.CreateGameObject<RenderableGameObject>(name);

            // Configurar todas las mallas del modelo
            for (size_t i = 0; i < model->meshes.size(); i++) {
                obj->SetMeshIndex(i);
            }

            // Configurar transformaciones
            obj->SetPosition(glm::vec3(0.0f));
            obj->SetRotation(glm::vec3(0.0f));
            obj->SetScale(glm::vec3(1.0f));

            return obj;
        }
    }

    std::cerr << "Error: Could not load primitive mesh from file: " << meshFile << std::endl;
    return nullptr;
}
Camera* PrimitiveFactory::CreateCamera() {
    // Crear un objeto GameObjectCamera   
    auto& manager = GameObjectManager::GetInstance();
    
    // Crear la c�mara usando el manager
    GameObjectCamera* camera = manager.CreateGameObject<GameObjectCamera>("MainCamera");


    // Configurar su posici�n y rotaci�n si es necesario
    camera->SetPosition(glm::vec3(0.0f, 2.0f, 5.0f)); // Posici�n inicial
    camera->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f)); // Rotaci�n inicial

    return camera; // Retornamos un puntero a Camera*
}

