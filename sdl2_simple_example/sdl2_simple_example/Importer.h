#pragma once

#include <GL/glew.h>                  
#include <string>                   
#include <vector>                     
#include <filesystem>                
#include <assimp/Importer.hpp>        
#include <assimp/scene.h>            
#include <assimp/postprocess.h>    
#include <unordered_map>

class Importer {
public:
    struct Mesh {
        std::vector<GLfloat> vertices;
        std::vector<GLfloat> texCoords;
        std::vector<GLuint> indices;

        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;
    };

    struct Model {
        std::string name;
        std::vector<Mesh> meshes;
    };

    struct Texture {
        GLuint textureID = 0;
        int width = 0;
        int height = 0;
        int channels = 0;
        std::string name;
    };

    // Constructor y destructor
    Importer();
    ~Importer();

    bool Init();

    // Funciones modificadas para manejo de modelos
    bool ImportFBX(const std::string& filePath);
    bool SaveModelToCustomFormat(const std::string& modelName, const std::string& outputPath);
    bool LoadModelFromCustomFormat(const std::string& filePath);

    // Funciones para manejo de texturas
    bool ImportTexture(const std::string& filePath);
    const std::string GetTextureName(const std::string& filepath)const;
    const Texture* GetTexture(const std::string& textureName)const;

    // Getters modificados
    const std::unordered_map<std::string, Model>& GetModels() const { return models; }
    const Model* GetModel(const std::string& modelName) const;
    const std::string GetModelName(const std::string& filePath) const;
    const Texture& GetTexture() const { return texture; }
    GLuint GetTextureID() const { return texture.textureID; }

private:
    bool CreateRequiredDirectories();
    bool ProcessNode(aiNode* node, const aiScene* scene, Model& model);
    bool ProcessMesh(aiMesh* mesh, const aiScene* scene, Model& model);

    std::unordered_map<std::string, Model> models; 
    std::unordered_map<std::string, Texture> textures;
    Texture texture;
    std::string assetsPath;
    std::string libraryPath;
};
