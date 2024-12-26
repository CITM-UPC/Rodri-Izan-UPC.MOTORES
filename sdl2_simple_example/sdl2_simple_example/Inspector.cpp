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

        // Active checkbox
        bool isActive = selectedGameObject->IsActive();
        if (ImGui::Checkbox("Active", &isActive)) {
            selectedGameObject->SetActive(isActive);
        }

        // Local Transform
        ImGui::Text("Local Transform");
        glm::vec3 localPosition = selectedGameObject->GetLocalPosition();
        glm::vec3 localRotation = selectedGameObject->GetLocalRotation();
        glm::vec3 localScale = selectedGameObject->GetLocalScale();

        bool changed = false;
        changed |= ImGui::DragFloat3("Local Position", &localPosition[0], 0.1f);
        changed |= ImGui::DragFloat3("Local Rotation", &localRotation[0], 0.1f);
        changed |= ImGui::DragFloat3("Local Scale", &localScale[0], 0.1f);

        if (changed) {
            selectedGameObject->SetLocalPosition(localPosition);
            selectedGameObject->SetLocalRotation(localRotation);
            selectedGameObject->SetLocalScale(localScale);
        }

        // Global Transform (read-only)
        ImGui::Separator();
        ImGui::Text("Global Transform");
        glm::vec3 globalPosition = selectedGameObject->GetGlobalPosition();
        ImGui::Text("Global Position: %.2f, %.2f, %.2f",
            globalPosition.x, globalPosition.y, globalPosition.z);

        // Hierarchy Information
        ImGui::Separator();
        if (GameObject* parent = selectedGameObject->GetParent()) {
            ImGui::Text("Parent: %s", parent->GetName().c_str());
            if (ImGui::Button("Remove Parent")) {
                parent->RemoveChild(selectedGameObject);
            }
        }
        else {
            ImGui::Text("No Parent");
        }

        // Children
        const auto& children = selectedGameObject->GetChildren();
        if (!children.empty()) {
            ImGui::Text("Children:");
            for (const auto* child : children) {
                ImGui::BulletText("%s", child->GetName().c_str());
            }
        }
        else {
            ImGui::Text("No Children");
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
