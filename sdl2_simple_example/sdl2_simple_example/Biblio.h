#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>

class Biblio {
public:
    Biblio(); // Constructor predeterminado
    Biblio(const std::string& assetsPath); // Constructor que recibe una ruta de assets

    void DrawAssetsWindow(); // Dibujar la ventana de assets

    // Getter para obtener el asset seleccionado
    const std::string& GetSelectedAsset() const;

private:
    std::string m_assetsPath = "Assets/"; // Ruta predeterminada
    std::string m_selectedAsset;          // Asset actualmente seleccionado

    void DrawDirectoryRecursive(const std::string& path);
    GLuint LoadTexture(const std::string& path);
};
