#include "Inspector.h"
#include "imgui.h"

Inspector::Inspector() : selectedGameObject(-1), isActive(true) {}

void Inspector::DrawInspectorWindow() {
    ImGui::Begin("Inspector");
    if (selectedGameObject != -1) {
        ImGui::Text("Properties of GameObject %d", selectedGameObject);
        ImGui::Checkbox("Active", &isActive);
        if (ImGui::CollapsingHeader("Transform")) {
            static float position[3] = { 0.0f, 0.0f, 0.0f };
            static float rotation[3] = { 0.0f, 0.0f, 0.0f };
            static float scale[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::DragFloat3("Position", position);
            ImGui::DragFloat3("Rotation", rotation);
            ImGui::DragFloat3("Scale", scale);
        }
        if (ImGui::CollapsingHeader("Mesh")) {
            ImGui::Text("Mesh properties here");
        }
        if (ImGui::CollapsingHeader("Material")) {
            ImGui::Text("Material properties here");
        }
    }
    else {
        ImGui::Text("Select a GameObject to see details.");
    }
    ImGui::End();
}