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

Camera* PrimitiveFactory::CreateCamera(Importer* importer) {
    auto& manager = GameObjectManager::GetInstance();
    GameObjectCamera* camera = manager.CreateGameObject<GameObjectCamera>("MainCamera");

    const std::string cameraModelPath = "Assets/Models/Camera.fbx";
    if (importer && importer->ImportFBX(cameraModelPath.c_str())) {
        const std::string modelName = importer->GetModelName(cameraModelPath.c_str());
        const auto* model = importer->GetModel(modelName);
        if (model && !model->meshes.empty()) {
            // Configurar las mallas del modelo
            for (size_t i = 0; i < model->meshes.size(); i++) {
                camera->SetMeshIndex(i);
                // Verificar que el material se asigna correctamente
                std::cout << "Mesh " << i << " assigned to camera" << std::endl;
                // Si tu sistema usa materiales, asegúrate de que se asignen aquí
            }
        }
        camera->SetPosition(glm::vec3(0.0f, 2.0f, 5.0f));
        camera->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
        camera->SetScale(glm::vec3(0.1f)); // Añade una escala si el modelo es muy grande
    }
    return camera;
}