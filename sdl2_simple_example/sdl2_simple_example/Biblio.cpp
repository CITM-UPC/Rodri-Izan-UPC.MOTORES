#include "Biblio.h"
#include <filesystem>
#include <stb_image.h>
#include <iostream>
#include "imgui.h"
#include <GL/glew.h>

namespace fs = std::filesystem;

Biblio::Biblio()
    : m_assetsPath("./Assets"),
    m_selectedAsset("") {}

Biblio::Biblio(const std::string& assetsPath)
    : m_assetsPath(assetsPath), m_selectedAsset("") {}


void Biblio::DrawAssetsWindow() {
    ImGui::Begin("Assets");

    // Mostrar archivo seleccionado actualmente
    if (!m_selectedAsset.empty()) {
        ImGui::Text("Selected Asset: %s", m_selectedAsset.c_str());
    }
    else {
        ImGui::Text("No asset selected");
    }

    // Verificar que la ruta de assets sea válida
    if (fs::exists(m_assetsPath)) {
        if (fs::is_directory(m_assetsPath)) {
            ImGui::Text("Valid assets path: %s", m_assetsPath.c_str());
            DrawDirectoryRecursive(m_assetsPath);
        }
        else {
            ImGui::Text("Path exists but is not a directory: %s", m_assetsPath.c_str());
        }
    }
    else {
        ImGui::Text("Path does not exist: %s", m_assetsPath.c_str());
    }

    ImGui::End();
}

void Biblio::DrawDirectoryRecursive(const std::string& path) {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            if (ImGui::TreeNode(entry.path().filename().string().c_str())) {
                DrawDirectoryRecursive(entry.path().string());
                ImGui::TreePop();
            }
        }
        else {
            if (ImGui::Selectable(entry.path().filename().string().c_str(),
                m_selectedAsset == entry.path().string())) {
                m_selectedAsset = entry.path().string();
            }

           
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                std::string filePath = entry.path().string();
                const char* filePathCStr = filePath.c_str();
                ImGui::SetDragDropPayload(GetDragDropType(), filePathCStr, strlen(filePathCStr) + 1);
                ImGui::Text("Dragging: %s", entry.path().filename().string().c_str());
                ImGui::EndDragDropSource();
            }
        }
    }
}
GLuint Biblio::LoadTexture(const std::string& path) {
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
        channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return textureID;
}
