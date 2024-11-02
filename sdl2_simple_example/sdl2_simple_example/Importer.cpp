#include "Importer.h"
#include <chrono>
#include <iostream>
#include <fstream> 

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Importer::Importer() :
    assetsPath("Assets/"),
    libraryPath("Library/") {
}

Importer::~Importer() {
    // Liberar recursos de textura si están en uso
    if (texture.textureID != 0) {
        glDeleteTextures(1, &texture.textureID);
    }

    // Liberar recursos de mallas (meshes) si están en uso
    for (auto& mesh : meshes) {
        if (mesh.VAO != 0) glDeleteVertexArrays(1, &mesh.VAO);  
        if (mesh.VBO != 0) glDeleteBuffers(1, &mesh.VBO);       
        if (mesh.EBO != 0) glDeleteBuffers(1, &mesh.EBO);       
    }
}

bool Importer::Init() {
    return CreateRequiredDirectories();
}

bool Importer::CreateRequiredDirectories() {
    namespace fs = std::filesystem;  

    try {
        // Lista de directorios necesarios
        std::vector<std::string> directories = {
            assetsPath,
            libraryPath + "Meshes",
            libraryPath + "Materials",
            libraryPath + "Models"
        };

        // Crear cada directorio si no existe
        for (const auto& dir : directories) {
            if (!fs::exists(dir)) {
                fs::create_directories(dir);
            }
        }
        return true;
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error creating directories: " << e.what() << std::endl;
        return false;
    }
}

bool Importer::ImportFBX(const std::string& filePath) {
    // Construir la ruta del archivo custom
    std::string customPath = "Library/Meshes/" +
        std::filesystem::path(filePath).stem().string() + ".custom";

    // Intentar cargar el archivo custom primero
    if (std::filesystem::exists(customPath)) {
        std::cout << "Custom format found, loading " << customPath << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();

        bool success = LoadMeshFromCustomFormat(customPath);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "Custom format loading time: " << duration.count() << "ms" << std::endl;

        return success;
    }

    // Si no existe el archivo custom, procesar el FBX
    std::cout << "No custom format found, processing FBX..." << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading FBX: " << importer.GetErrorString() << std::endl;
        return false;
    }

    meshes.clear();
    bool success = ProcessNode(scene->mRootNode, scene);

    // Si el procesamiento fue exitoso, guardar en formato custom
    if (success) {
        SaveMeshToCustomFormat(customPath);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "FBX processing time: " << duration.count() << "ms" << std::endl;

    return success;
}

bool Importer::ProcessNode(aiNode* node, const aiScene* scene) {
    // Procesar cada malla en el nodo
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);  // Llamar al procesamiento de la malla
    }

    // Procesar los nodos hijos
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }

    return true;
}

bool Importer::ProcessMesh(aiMesh* aimesh, const aiScene* scene) {
    Mesh mesh;  

    // Recorrer vértices de la malla y extraer datos de posición y textura
    for (unsigned int i = 0; i < aimesh->mNumVertices; i++) {
        // Agregar coordenadas de posición
        mesh.vertices.push_back(aimesh->mVertices[i].x);
        mesh.vertices.push_back(aimesh->mVertices[i].y);
        mesh.vertices.push_back(aimesh->mVertices[i].z);

        // Agregar coordenadas de textura si existen; si no, añadir (0, 0)
        if (aimesh->HasTextureCoords(0)) {
            mesh.texCoords.push_back(aimesh->mTextureCoords[0][i].x);
            mesh.texCoords.push_back(aimesh->mTextureCoords[0][i].y);
        }
        else {
            mesh.texCoords.push_back(0.0f);
            mesh.texCoords.push_back(0.0f);
        }
    }

    // Recorrer cada cara y extraer los índices de vértices para la estructura interna
    for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
        const aiFace& face = aimesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            mesh.indices.push_back(face.mIndices[j]);
        }
    }

    meshes.push_back(mesh);  
    return true;
}

bool Importer::SaveMeshToCustomFormat(const std::string& outputPath) {
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for saving: " << outputPath << std::endl;
        return false;
    }

    size_t meshCount = meshes.size();
    outFile.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

    for (const auto& mesh : meshes) {
        size_t vertexCount = mesh.vertices.size();
        size_t texCoordCount = mesh.texCoords.size();
        size_t indexCount = mesh.indices.size();

        outFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
        outFile.write(reinterpret_cast<const char*>(mesh.vertices.data()), vertexCount * sizeof(GLfloat));

        outFile.write(reinterpret_cast<const char*>(&texCoordCount), sizeof(texCoordCount));
        outFile.write(reinterpret_cast<const char*>(mesh.texCoords.data()), texCoordCount * sizeof(GLfloat));

        outFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
        outFile.write(reinterpret_cast<const char*>(mesh.indices.data()), indexCount * sizeof(GLuint));
    }

    return true;
}

bool Importer::LoadMeshFromCustomFormat(const std::string& filePath) {
    auto startTime = std::chrono::high_resolution_clock::now();  // Medir tiempo de inicio
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error opening file for loading: " << filePath << std::endl;
        return false;
    }

    meshes.clear();
    size_t meshCount;
    inFile.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));

    for (size_t i = 0; i < meshCount; i++) {
        Mesh mesh;
        size_t vertexCount, texCoordCount, indexCount;

        inFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
        mesh.vertices.resize(vertexCount);
        inFile.read(reinterpret_cast<char*>(mesh.vertices.data()), vertexCount * sizeof(GLfloat));

        inFile.read(reinterpret_cast<char*>(&texCoordCount), sizeof(texCoordCount));
        mesh.texCoords.resize(texCoordCount);
        inFile.read(reinterpret_cast<char*>(mesh.texCoords.data()), texCoordCount * sizeof(GLfloat));

        inFile.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
        mesh.indices.resize(indexCount);
        inFile.read(reinterpret_cast<char*>(mesh.indices.data()), indexCount * sizeof(GLuint));

        meshes.push_back(mesh);
    }

    // Calcular tiempo de carga y mostrarlo
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "FBX Loading time: " << duration.count() << "ms" << std::endl;

    return true;
}

bool Importer::ImportTexture(const std::string& filePath) {
    if (texture.textureID != 0) {
        glDeleteTextures(1, &texture.textureID);
    }

    // Cargar nueva textura usando stb_image
    unsigned char* data = stbi_load(filePath.c_str(),
        &texture.width, &texture.height, &texture.channels, STBI_rgb_alpha);

    if (!data) {
        std::cerr << "Error loading texture: " << filePath << std::endl;
        return false;
    }

    // Generar y configurar la textura 
    glGenTextures(1, &texture.textureID);
    glBindTexture(GL_TEXTURE_2D, texture.textureID);

    // Parámetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Filtro min
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Filtro mag

    // Cargar la imagen en OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);  // Liberar datos de la imagen cargada
    return true;
}
