#include "Importer.h"
#include <chrono>
#include <iostream>

Importer::Importer() :
    assetsPath("Assets/"),
    libraryPath("Library/") {
}

Importer::~Importer() {
    if (texture.textureID != 0) {
        glDeleteTextures(1, &texture.textureID);
    }

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
        std::vector<std::string> directories = { 
            assetsPath,
            libraryPath + "Meshes",
            libraryPath + "Materials",
            libraryPath + "Models"
        };

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

    auto endTime = std::chrono::high_resolution_clock::now(); 
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "FBX Loading time: " << duration.count() << "ms" << std::endl;

    return success;
}

bool Importer::ProcessNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }

    return true;
}

bool Importer::ProcessMesh(aiMesh* aimesh, const aiScene* scene) {
    Mesh mesh;

    for (unsigned int i = 0; i < aimesh->mNumVertices; i++) {
        mesh.vertices.push_back(aimesh->mVertices[i].x);
        mesh.vertices.push_back(aimesh->mVertices[i].y);
        mesh.vertices.push_back(aimesh->mVertices[i].z);

        if (aimesh->HasTextureCoords(0)) {
            mesh.texCoords.push_back(aimesh->mTextureCoords[0][i].x);
            mesh.texCoords.push_back(aimesh->mTextureCoords[0][i].y);
        }
        else {
            mesh.texCoords.push_back(0.0f);
            mesh.texCoords.push_back(0.0f);
        }
    }

    for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
        const aiFace& face = aimesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            mesh.indices.push_back(face.mIndices[j]);
        }
    }

    meshes.push_back(mesh);
    return true;
}

bool Importer::ImportTexture(const std::string& filePath) {
    // Liberar textura anterior si existe
    if (texture.textureID != 0) {
        glDeleteTextures(1, &texture.textureID);
    }

    // Cargar nueva textura
    unsigned char* data = stbi_load(filePath.c_str(),
        &texture.width, &texture.height, &texture.channels, STBI_rgb_alpha);

    if (!data) {
        std::cerr << "Error loading texture: " << filePath << std::endl;
        return false;
    }

    glGenTextures(1, &texture.textureID);
    glBindTexture(GL_TEXTURE_2D, texture.textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return true;
}