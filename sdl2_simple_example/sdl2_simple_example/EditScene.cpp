// EditScene.cpp
#include "EditScene.h"
#include "imgui.h"

// Definición de variables globales
int selectedGameObject = -1;
bool isActive = true;

void DrawHierarchyWindow() {
    if (ImGui::Begin("Hierarchy")) { 
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
}


void DrawInspectorWindow() {
    ImGui::Begin("Inspector");
    if (selectedGameObject != -1) {
        ImGui::Text("Properties of GameObject %d", selectedGameObject);
        ImGui::Checkbox("Active", &isActive);
        if (ImGui::CollapsingHeader("Transform")) {
            ImGui::DragFloat3("Position", new float[3] {0.0f, 0.0f, 0.0f});
            ImGui::DragFloat3("Rotation", new float[3] {0.0f, 0.0f, 0.0f});
            ImGui::DragFloat3("Scale", new float[3] {1.0f, 1.0f, 1.0f});
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

void DrawAssetWindow() {
    ImGui::Begin("Assets");
    if (ImGui::TreeNode("Textures")) {
        ImGui::Text("texture1.png");
        ImGui::Text("texture2.jpg");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Scripts")) {
        ImGui::Text("PlayerController.cs");
        ImGui::Text("EnemyAI.cs");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Prefabs")) {
        ImGui::Text("Player.prefab");
        ImGui::Text("Enemy.prefab");
        ImGui::TreePop();
    }
    ImGui::End();
}

void RenderEditor() {
    DrawHierarchyWindow();
    DrawInspectorWindow();
    DrawAssetWindow();
}
