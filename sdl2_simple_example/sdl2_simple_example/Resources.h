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

    // M�todo para mostrar la ventana
    void Display() {
            // Bot�n de recarga
            if (ImGui::Button("Recargar")) {
                UpdateResourceUsage(); // Actualizar los datos al presionar el bot�n
            }

            // Mostrar informaci�n de mallas
            if (!m_meshUsage.empty()) {
                ImGui::Text("Mallas:");
                for (const auto& pair : m_meshUsage) {
                    ImGui::Text("  Mesh ID %d: %d usos", pair.first, pair.second);
                }
            }
            else {
                ImGui::Text("No se encontraron mallas.");
            }

            // Mostrar informaci�n de texturas
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
    // M�todo para actualizar los datos de uso de recursos
    void UpdateResourceUsage() {
        // Obtener todos los GameObjects del GameObjectManager
        auto& manager = GameObjectManager::GetInstance();
        const auto& gameObjects = manager.GetAllGameObjects();

        // Reiniciar los contadores
        m_meshUsage.clear();
        m_textureUsage.clear();

        // Contar cu�ntas veces se usa cada recurso
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
    std::unordered_map<int, int> m_meshUsage;       // Mapa de ID de malla a n�mero de usos
    std::unordered_map<GLuint, int> m_textureUsage; // Mapa de ID de textura a n�mero de usos
    bool m_needsUpdate;                             // Bandera para indicar si se necesita actualizaci�n
};
