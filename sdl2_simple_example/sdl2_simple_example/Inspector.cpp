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

        bool isActive = selectedGameObject->IsActive();
        if (ImGui::Checkbox("Active", &isActive)) {
            selectedGameObject->SetActive(isActive);
        }

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
    }
    else {
        ImGui::Text("Select a GameObject to see details.");
    }

    ImGui::End();
}