#include "Hierarchy.h"
#include "GameObjectManager.h"
#include "imgui.h"

Hierarchy::Hierarchy() : selectedGameObject(-1) {}

void Hierarchy::DrawHierarchyWindow() {
    ImGui::Begin("Hierarchy");

    auto& gameObjectManager = GameObjectManager::GetInstance();
    auto allGameObjects = gameObjectManager.GetAllGameObjects();

    for (GameObject* gameObject : allGameObjects) {
        bool isSelected = gameObjectManager.GetSelectedGameObject() == gameObject;
        if (ImGui::TreeNodeEx(gameObject->GetName().c_str(), isSelected ? ImGuiTreeNodeFlags_Selected : 0)) {
            if (ImGui::IsItemClicked()) {
                gameObjectManager.SetSelectedGameObject(gameObject);
            }
            // Render children, if any
            ImGui::TreePop();
        }
    }

    ImGui::End();
}