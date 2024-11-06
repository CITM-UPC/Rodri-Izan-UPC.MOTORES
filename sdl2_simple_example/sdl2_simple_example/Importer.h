#pragma once

#include <GL/glew.h>                  
#include <string>                   
#include <vector>                     
#include <filesystem>                
#include <assimp/Importer.hpp>        
#include <assimp/scene.h>            
#include <assimp/postprocess.h>    

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


    struct Texture {
        GLuint textureID = 0; 
        int width = 0;        
        int height = 0;       
        int channels = 0;   
    };

    // Constructor y destructor
    Importer(); 
    ~Importer(); 

    bool Init(); 

    // Funciones para manejo de mallas
    bool ImportFBX(const std::string& filePath);          // Importar malla desde un archivo FBX
    bool SaveMeshToCustomFormat(const std::string& outputPath); // Guardar malla en formato personalizado
    bool LoadMeshFromCustomFormat(const std::string& filePath); // Cargar malla desde formato personalizado

    // Funciones para manejo de texturas
    bool ImportTexture(const std::string& filePath);          // Importar textura desde archivo
    //bool SaveTextureToCustomFormat(const std::string& outputPath); // Guardar textura en formato personalizado
    //bool LoadTextureFromCustomFormat(const std::string& filePath); // Cargar textura desde formato personalizado

    // Getters para obtener información de las mallas y texturas cargadas
    const std::vector<Mesh>& GetMeshes() const { return meshes; }  // Obtener mallas cargadas
    const Texture& GetTexture() const { return texture; }           // Obtener textura cargada
    GLuint GetTextureID() const { return texture.textureID; }       // Obtener ID de textura para OpenGL

private:
    // Funciones internas
    bool CreateRequiredDirectories();                     // Crear directorios requeridos para guardar recursos
    bool ProcessNode(aiNode* node, const aiScene* scene); // Procesar un nodo de la escena 
    bool ProcessMesh(aiMesh* mesh, const aiScene* scene); // Procesar una malla 


    std::vector<Mesh> meshes;       
    Texture texture;             
    std::string assetsPath;         
    std::string libraryPath;       
};
