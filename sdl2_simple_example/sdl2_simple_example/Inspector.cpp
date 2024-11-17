#include "Inspector.h"
#include "GameObjectManager.h"
#include "imgui.h"

Inspector::Inspector() {}

void Inspector::DrawInspectorWindow() {
    ImGui::Begin("Inspector");

    auto& gameObjectManager = GameObjectManager::GetInstance();
    GameObject* selectedGameObject = gameObjectManager.GetSelectedGameObject();

    if (selectedGameObject) {
        ImGui::Text("Properties of %s", selectedGameObject->GetName().c_str());

        // Checkbox para activar/desactivar el objeto
        bool isActive = selectedGameObject->IsActive();
        if (ImGui::Checkbox("Active", &isActive)) {
            selectedGameObject->SetActive(isActive);
        }

        // Transformaciones básicas
        glm::vec3 position = selectedGameObject->GetPosition();
        glm::vec3 rotation = selectedGameObject->GetRotation();
        glm::vec3 scale = selectedGameObject->GetScale();
        if (ImGui::DragFloat3("Position", &position[0], 0.1f)) {
            selectedGameObject->SetPosition(position);
        }
        if (ImGui::DragFloat3("Rotation", &rotation[0], 0.1f)) {
            selectedGameObject->SetRotation(rotation);
        }
        if (ImGui::DragFloat3("Scale", &scale[0], 0.1f)) {
            selectedGameObject->SetScale(scale);
        }

        // Si el objeto es RenderableGameObject
        if (auto renderable = dynamic_cast<RenderableGameObject*>(selectedGameObject)) {
            ImGui::Separator();
            ImGui::Text("Rendering Properties");

            GLuint textureID = renderable->GetTextureID();

            // Mostrar la textura actual si existe
            if (textureID != 0) {
                ImGui::Text("Current Texture:");
                ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(textureID)), ImVec2(100, 100));
            }
            else {
                ImGui::Text("No Texture Assigned");
            }

            //// Botón para cambiar la textura
            //if (ImGui::Button("Change Texture")) {
            //    // Aquí deberías implementar un diálogo o un mecanismo para cargar texturas
            //    GLuint newTextureID = LoadNewTexture(); // Implementa esta función
            //    if (newTextureID != 0) {
            //        renderable->SetTextureID(newTextureID);
            //    }
            //}
        }
    }
    else {
        ImGui::Text("Select a GameObject to see details.");
    }

    ImGui::End();
}
