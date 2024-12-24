#include "Hierarchy.h"
#include "Empty.h"
#include "GameObjectManager.h"
#include "imgui.h"

Hierarchy::Hierarchy() : selectedGameObject(-1) {}

void Hierarchy::DrawHierarchyWindow() {
    ImGui::Begin("Hierarchy");
    auto& gameObjectManager = GameObjectManager::GetInstance();
    auto allGameObjects = gameObjectManager.GetAllGameObjects();

    // Solo dibuja GameObjects sin padres (raíz de la jerarquía)
    for (GameObject* gameObject : allGameObjects) {
        if (!gameObject->GetParent()) {
            DrawGameObjectNode(gameObject);
        }
    }

    // Menú contextual para crear Empty
    if (ImGui::BeginPopupContextWindow("HierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Create Empty")) {
            gameObjectManager.CreateGameObject("Empty");
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void Hierarchy::DrawGameObjectNode(GameObject* gameObject) {
    if (!gameObject) return;

    auto& gameObjectManager = GameObjectManager::GetInstance();
    bool isSelected = gameObjectManager.GetSelectedGameObject() == gameObject;

    // Configurar flags para el TreeNode
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;
    if (gameObject->GetChildren().empty()) flags |= ImGuiTreeNodeFlags_Leaf;

    bool opened = ImGui::TreeNodeEx(gameObject->GetName().c_str(), flags);

    // Selección del objeto
    if (ImGui::IsItemClicked()) {
        gameObjectManager.SetSelectedGameObject(gameObject);
    }

    // Drag & Drop source (cuando arrastramos)
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        const char* gameObjectPtr = reinterpret_cast<const char*>(&gameObject);
        ImGui::SetDragDropPayload("GAMEOBJECT", gameObjectPtr, sizeof(GameObject*));
        ImGui::Text("Moving: %s", gameObject->GetName().c_str());
        ImGui::EndDragDropSource();
    }

    // Drag & Drop target (cuando soltamos)
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
            GameObject* draggedObject = *reinterpret_cast<GameObject**>(payload->Data);

            // Evitar ciclos y auto-referencia
            bool canSetParent = true;
            GameObject* parent = gameObject;
            while (parent) {
                if (parent == draggedObject) {
                    canSetParent = false;
                    break;
                }
                parent = parent->GetParent();
            }

            if (canSetParent && draggedObject != gameObject) {
                // Remover del padre anterior si existe
                if (GameObject* oldParent = draggedObject->GetParent()) {
                    oldParent->RemoveChild(draggedObject);
                }

                // Establecer nuevo padre
                gameObject->AddChild(draggedObject);
            }
        }
        ImGui::EndDragDropTarget();
    }

    // Menú contextual para cada GameObject
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete")) {
            gameObjectManager.DestroyGameObject(gameObject->GetName());
        }
        if (ImGui::MenuItem("Remove Parent") && gameObject->GetParent()) {
            gameObject->GetParent()->RemoveChild(gameObject);
        }
        ImGui::EndPopup();
    }

    // Dibujar hijos recursivamente
    if (opened) {
        for (GameObject* child : gameObject->GetChildren()) {
            DrawGameObjectNode(child);
        }
        ImGui::TreePop();
    }
}

