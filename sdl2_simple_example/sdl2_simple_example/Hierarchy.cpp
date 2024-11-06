#include "Hierarchy.h"
#include "imgui.h"

Hierarchy::Hierarchy() : selectedGameObject(-1) {}

void Hierarchy::DrawHierarchyWindow() {
    ImGui::Begin("Hierarchy");
    if (ImGui::TreeNodeEx("GameObject 1", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::IsItemClicked()) {
            selectedGameObject = 1;
        }
        if (ImGui::TreeNode("Child 1")) {
            if (ImGui::IsItemClicked()) {
                selectedGameObject = 2;
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    ImGui::End();
}
