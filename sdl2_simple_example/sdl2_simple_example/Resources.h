#pragma once
#include "GameObjectManager.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <imgui.h>

class ResourceUsageWindow {
public:
    // Constructor
    ResourceUsageWindow() : m_needsUpdate(true) {}

    // Método para mostrar la ventana
    void Display() {
            // Botón de recarga
            if (ImGui::Button("Recargar")) {
                UpdateResourceUsage(); // Actualizar los datos al presionar el botón
            }

            // Mostrar información de mallas
            if (!m_meshUsage.empty()) {
                ImGui::Text("Mallas:");
                for (const auto& pair : m_meshUsage) {
                    ImGui::Text("  Mesh ID %d: %d usos", pair.first, pair.second);
                }
            }
            else {
                ImGui::Text("No se encontraron mallas.");
            }

            // Mostrar información de texturas
            if (!m_textureUsage.empty()) {
                ImGui::Text("Texturas:");
                for (const auto& pair : m_textureUsage) {
                    ImGui::Text("  Texture ID %u: %d usos", pair.first, pair.second);
                }
            }
            else {
                ImGui::Text("No se encontraron texturas.");
            }
    }

private:
    // Método para actualizar los datos de uso de recursos
    void UpdateResourceUsage() {
        // Obtener todos los GameObjects del GameObjectManager
        auto& manager = GameObjectManager::GetInstance();
        const auto& gameObjects = manager.GetAllGameObjects();

        // Reiniciar los contadores
        m_meshUsage.clear();
        m_textureUsage.clear();

        // Contar cuántas veces se usa cada recurso
        for (const auto* gameObject : gameObjects) {
            if (const auto* renderable = dynamic_cast<const RenderableGameObject*>(gameObject)) {
                // Contar mallas
                for (int meshIndex : renderable->GetMeshIndices()) {
                    m_meshUsage[meshIndex]++;
                }
                // Contar texturas
                GLuint textureID = renderable->GetTextureID();
                if (textureID != 0) {
                    m_textureUsage[textureID]++;
                }
            }
        }
    }

    // Atributos privados
    std::unordered_map<int, int> m_meshUsage;       // Mapa de ID de malla a número de usos
    std::unordered_map<GLuint, int> m_textureUsage; // Mapa de ID de textura a número de usos
    bool m_needsUpdate;                             // Bandera para indicar si se necesita actualización
};
