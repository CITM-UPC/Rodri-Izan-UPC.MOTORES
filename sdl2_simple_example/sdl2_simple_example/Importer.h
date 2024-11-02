#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

class Importer {
public:
    struct Mesh {
        std::vector<GLfloat> vertices;
        std::vector<GLfloat> texCoords;
        std::vector<GLuint> indices;
        GLuint VAO = 0;// Vertex Array Object
        GLuint VBO = 0;// Vertex Buffer Object
        GLuint EBO = 0;// Element Buffer Object
    };

    struct Texture {
        GLuint textureID = 0;
        int width = 0;
        int height = 0;
        int channels = 0;
    };

    // Constructor and destructor
    Importer();
    ~Importer();

    // Initialization
    bool Init();

    // Mesh functions
    bool ImportFBX(const std::string& filePath);
    bool SaveMeshToCustomFormat(const std::string& outputPath);
    bool LoadMeshFromCustomFormat(const std::string& filePath);

    // Texture functions
    bool ImportTexture(const std::string& filePath);
    bool SaveTextureToCustomFormat(const std::string& outputPath);
    bool LoadTextureFromCustomFormat(const std::string& filePath);

    // Getters
    const std::vector<Mesh>& GetMeshes() const { return meshes; }
    const Texture& GetTexture() const { return texture; }
    GLuint GetTextureID() const { return texture.textureID; }

private:
    bool CreateRequiredDirectories();
    bool ProcessNode(aiNode* node, const aiScene* scene);
    bool ProcessMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Mesh> meshes;
    Texture texture;
    std::string assetsPath;
    std::string libraryPath;
};