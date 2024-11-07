#include "Biblio.h"
#include "imgui.h"

Biblio::Biblio() {}

void Biblio::DrawTransformWindow() {
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