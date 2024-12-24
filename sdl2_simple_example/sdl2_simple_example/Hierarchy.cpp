#include "Hierarchy.h"
#include "GameObjectManager.h"
#include "imgui.h"

Hierarchy::Hierarchy() : selectedGameObject(-1) {}

void Hierarchy::DrawHierarchyWindow() {
    ImGui::Begin("Hierarchy");

    auto& gameObjectManager = GameObjectManager::GetInstance();
    auto allGameObjects = gameObjectManager.GetAllGameObjects();

    for (GameObject* gameObject : allGameObjects) {
        // Solo dibuja GameObjects sin padres (raíz de la jerarquía)
        if (!gameObject->GetParent()) {
            DrawGameObjectNode(gameObject);
        }
    }

    if (ImGui::BeginPopupContextWindow("HierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Create Empty")) {
            gameObjectManager.CreateGameObject("Empty");
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void Hierarchy::DrawGameObjectNode(GameObject* gameObject) {
    auto& gameObjectManager = GameObjectManager::GetInstance();
    bool isSelected = gameObjectManager.GetSelectedGameObject() == gameObject;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;
    if (gameObject->GetChildren().empty()) flags |= ImGuiTreeNodeFlags_Leaf;

    bool opened = ImGui::TreeNodeEx(gameObject->GetName().c_str(), flags);
    if (ImGui::IsItemClicked()) {
        gameObjectManager.SetSelectedGameObject(gameObject);
    }

    // Arrastrar (Drag) el GameObject
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("HierarchyNode", &gameObject, sizeof(GameObject*)); // Asigna el payload
        ImGui::Text("Dragging %s", gameObject->GetName().c_str());
        ImGui::EndDragDropSource();
    }

    // Soltar (Drop) el GameObject sobre otro para hacerlo hijo
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HierarchyNode")) {
            GameObject* draggedGameObject = *(GameObject**)payload->Data;
            if (draggedGameObject != gameObject) { // Evita que un objeto sea hijo de sí mismo
                draggedGameObject->SetParent(gameObject);
            }
        }
        ImGui::EndDragDropTarget();
    }

    // Menú contextual para el GameObject
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete")) {
            gameObjectManager.DestroyGameObject(gameObject->GetName());
        }
        if (ImGui::BeginMenu("Set Parent")) {
            for (GameObject* potentialParent : gameObjectManager.GetAllGameObjects()) {
                if (potentialParent != gameObject) {
                    if (ImGui::MenuItem(potentialParent->GetName().c_str())) {
                        gameObject->SetParent(potentialParent);
                    }
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Remove Parent")) {
            gameObject->SetParent(nullptr);
        }
        ImGui::EndPopup();
    }

    // Renderizar los hijos del GameObject de manera recursiva
    if (opened) {
        for (GameObject* child : gameObject->GetChildren()) {
            DrawGameObjectNode(child);
        }
        ImGui::TreePop();
    }
}

