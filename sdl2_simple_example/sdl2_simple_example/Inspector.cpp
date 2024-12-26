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

        // Mostrar información sobre el parent si existe
        if (GameObject* parent = selectedGameObject->GetParent()) {
            ImGui::Text("Parent: %s", parent->GetName().c_str());
            if (ImGui::Button("Remove Parent")) {
                parent->RemoveChild(selectedGameObject);
            }
        }
        else {
            ImGui::Text("No Parent");
        }

        // Mostrar transformaciones locales
        ImGui::Text("Local Transform");
        glm::vec3 localPosition = selectedGameObject->GetPosition();
        glm::vec3 localRotation = selectedGameObject->GetRotation();
        glm::vec3 localScale = selectedGameObject->GetScale();

        if (ImGui::DragFloat3("Position", &localPosition[0], 0.1f)) {
            selectedGameObject->SetPosition(localPosition);
        }
        if (ImGui::DragFloat3("Rotation", &localRotation[0], 0.1f)) {
            selectedGameObject->SetRotation(localRotation);
        }
        if (ImGui::DragFloat3("Scale", &localScale[0], 0.1f)) {
            selectedGameObject->SetScale(localScale);
        }

        // Mostrar transformaciones globales (solo lectura)
        ImGui::Separator();
        ImGui::Text("Global Transform");
        glm::vec3 globalPosition = selectedGameObject->GetGlobalPosition();
        ImGui::Text("Global Position: %.2f, %.2f, %.2f",
            globalPosition.x, globalPosition.y, globalPosition.z);

        // Mostrar lista de hijos
        ImGui::Separator();
        ImGui::Text("Children");
        const auto& children = selectedGameObject->GetChildren();
        if (!children.empty()) {
            for (const auto* child : children) {
                ImGui::BulletText("%s", child->GetName().c_str());
            }
        }
        else {
            ImGui::Text("No children");
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
