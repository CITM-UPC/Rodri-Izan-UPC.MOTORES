#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>

class Biblio {
public:
    Biblio(); // Constructor predeterminado
    Biblio(const std::string& assetsPath); // Constructor que recibe una ruta de assets
    void DrawAssetsWindow();

private:
    std::string m_assetsPath = "Assets/"; // Ruta predeterminada
    void DrawDirectoryRecursive(const std::string& path);
    GLuint LoadTexture(const std::string& path);
};