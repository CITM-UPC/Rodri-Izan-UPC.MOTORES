#include "Hierarchy.h"
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

    // Manejar drag & drop en la ventana principal de jerarquía
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetDragDropType())) {
            if (payload->DataSize > 0) {
                const char* objectName = static_cast<const char*>(payload->Data);
                GameObject* draggedObject = gameObjectManager.FindGameObject(objectName);
                if (draggedObject && draggedObject->GetParent()) {
                    draggedObject->GetParent()->RemoveChild(draggedObject);
                }
            }
        }
        ImGui::EndDragDropTarget();
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

    // Drag & Drop source
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        std::string objectName = gameObject->GetName();
        const char* objectNameCStr = objectName.c_str();
        ImGui::SetDragDropPayload(GetDragDropType(), objectNameCStr, strlen(objectNameCStr) + 1);
        ImGui::Text("Moving: %s", objectName.c_str());
        ImGui::EndDragDropSource();
    }

    // Drag & Drop target
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetDragDropType())) {
            if (payload->DataSize > 0) {
                const char* draggedObjectName = static_cast<const char*>(payload->Data);
                GameObject* draggedObject = gameObjectManager.FindGameObject(draggedObjectName);

                if (draggedObject && draggedObject != gameObject) {
                    // Verificar que no estemos creando un ciclo
                    bool canSetParent = true;
                    GameObject* parent = gameObject;
                    while (parent) {
                        if (parent == draggedObject) {
                            canSetParent = false;
                            break;
                        }
                        parent = parent->GetParent();
                    }

                    if (canSetParent) {
                        // Guardar la posición global antes del cambio
                        glm::vec3 globalPos = draggedObject->GetGlobalPosition();

                        // Cambiar el padre
                        gameObject->AddChild(draggedObject);

                        // Ajustar la posición local para mantener la posición global
                        glm::vec4 newLocalPos = glm::inverse(gameObject->GetGlobalTransformMatrix()) *
                            glm::vec4(globalPos, 1.0f);
                        draggedObject->SetPosition(glm::vec3(newLocalPos));
                    }
                }
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