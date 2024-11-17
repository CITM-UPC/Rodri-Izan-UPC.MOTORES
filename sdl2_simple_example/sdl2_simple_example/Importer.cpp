#include "Importer.h"
#include <chrono>
#include <iostream>
#include <fstream> 
#include <filesystem>

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

    //// Liberar recursos de mallas (meshes) si están en uso
    //for (auto& mesh : meshes) {
    //    if (mesh.VAO != 0) glDeleteVertexArrays(1, &mesh.VAO);  
    //    if (mesh.VBO != 0) glDeleteBuffers(1, &mesh.VBO);       
    //    if (mesh.EBO != 0) glDeleteBuffers(1, &mesh.EBO);       
    //}
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
    std::string modelName = std::filesystem::path(filePath).stem().string();
    std::string customPath = "Library/Meshes/" + modelName + ".custom";

    // Verificar si el modelo ya está cargado
    if (models.find(modelName) != models.end()) {
        std::cout << "Model " << modelName << " already loaded" << std::endl;
        return true;
    }

    // Intentar cargar el archivo custom primero
    if (std::filesystem::exists(customPath)) {
        std::cout << "Custom format found, loading " << customPath << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();

        bool success = LoadModelFromCustomFormat(customPath);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "Custom format loading time: " << duration.count() << "ms" << std::endl;

        return success;
    }

    // Si no existe el archivo custom, procesar el FBX
    std::cout << "Processing FBX: " << modelName << std::endl;
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

    // Crear nuevo modelo
    Model newModel;
    newModel.name = modelName;

    bool success = ProcessNode(scene->mRootNode, scene, newModel);

    if (success) {
        // Agregar el nuevo modelo al mapa
        models[modelName] = std::move(newModel);
        // Guardar en formato custom
        SaveModelToCustomFormat(modelName, customPath);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "FBX processing time: " << duration.count() << "ms" << std::endl;

    return success;
}

bool Importer::ProcessNode(aiNode* node, const aiScene* scene, Model& model) {
    // Procesar cada malla en el nodo
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene, model);
    }

    // Procesar los nodos hijos
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, model);
    }

    return true;
}

bool Importer::ProcessMesh(aiMesh* aimesh, const aiScene* scene, Model& model) {
    try {
        Mesh mesh;

        // Verificar que el número de vértices sea razonable
        if (aimesh->mNumVertices > 1000000) {
            std::cerr << "Too many vertices in mesh: " << aimesh->mNumVertices << std::endl;
            return false;
        }

        // Reservar espacio para los vértices y coordenadas de textura
        mesh.vertices.reserve(aimesh->mNumVertices * 3);
        mesh.texCoords.reserve(aimesh->mNumVertices * 2);

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

        // Verificar que el número de caras sea razonable
        if (aimesh->mNumFaces > 1000000) {
            std::cerr << "Too many faces in mesh: " << aimesh->mNumFaces << std::endl;
            return false;
        }

        // Reservar espacio para los índices
        mesh.indices.reserve(aimesh->mNumFaces * 3);

        // Recorrer cada cara y extraer los índices de vértices
        for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
            const aiFace& face = aimesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                mesh.indices.push_back(face.mIndices[j]);
            }
        }

        model.meshes.push_back(std::move(mesh));
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while processing mesh: " << e.what() << std::endl;
        return false;
    }
}

bool Importer::SaveModelToCustomFormat(const std::string& modelName, const std::string& outputPath) {
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for saving: " << outputPath << std::endl;
        return false;
    }

    auto modelIt = models.find(modelName);
    if (modelIt == models.end()) {
        std::cerr << "Model " << modelName << " not found" << std::endl;
        return false;
    }

    const Model& model = modelIt->second;

    // Escribir el nombre del modelo
    size_t nameLength = model.name.length();
    outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    outFile.write(model.name.c_str(), nameLength);

    // Escribir el número de mallas
    size_t meshCount = model.meshes.size();
    outFile.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

    // Escribir cada malla
    for (const auto& mesh : model.meshes) {
        // Guardar vértices
        size_t vertexCount = mesh.vertices.size();
        outFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
        outFile.write(reinterpret_cast<const char*>(mesh.vertices.data()),
            vertexCount * sizeof(GLfloat));

        // Guardar coordenadas de textura
        size_t texCoordCount = mesh.texCoords.size();
        outFile.write(reinterpret_cast<const char*>(&texCoordCount), sizeof(texCoordCount));
        outFile.write(reinterpret_cast<const char*>(mesh.texCoords.data()),
            texCoordCount * sizeof(GLfloat));

        // Guardar índices
        size_t indexCount = mesh.indices.size();
        outFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
        outFile.write(reinterpret_cast<const char*>(mesh.indices.data()),
            indexCount * sizeof(GLuint));
    }

    std::cout << "Model " << modelName << " saved successfully to " << outputPath << std::endl;
    return true;
}

bool Importer::LoadModelFromCustomFormat(const std::string& filePath) {
    auto startTime = std::chrono::high_resolution_clock::now();
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error opening file for loading: " << filePath << std::endl;
        return false;
    }

    // Leer el nombre del modelo
    size_t nameLength;
    inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    std::string modelName(nameLength, '\0');
    inFile.read(&modelName[0], nameLength);

    // Crear nuevo modelo
    Model newModel;
    newModel.name = modelName;

    // Leer número de mallas
    size_t meshCount;
    inFile.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));

    // Leer cada malla
    for (size_t i = 0; i < meshCount; i++) {
        Mesh mesh;
        size_t vertexCount, texCoordCount, indexCount;

        // Leer vértices
        inFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
        mesh.vertices.resize(vertexCount);
        inFile.read(reinterpret_cast<char*>(mesh.vertices.data()),
            vertexCount * sizeof(GLfloat));

        // Leer coordenadas de textura
        inFile.read(reinterpret_cast<char*>(&texCoordCount), sizeof(texCoordCount));
        mesh.texCoords.resize(texCoordCount);
        inFile.read(reinterpret_cast<char*>(mesh.texCoords.data()),
            texCoordCount * sizeof(GLfloat));

        // Leer índices
        inFile.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
        mesh.indices.resize(indexCount);
        inFile.read(reinterpret_cast<char*>(mesh.indices.data()),
            indexCount * sizeof(GLuint));

        newModel.meshes.push_back(std::move(mesh));
    }

    // Agregar o actualizar el modelo en el mapa
    models[modelName] = std::move(newModel);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Custom format loading time: " << duration.count() << "ms" << std::endl;

    return true;
}

bool Importer::ImportTexture(const std::string& filePath) {
    std::string textureName = std::filesystem::path(filePath).stem().string();

    // Verificar si la textura ya está cargada
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        std::cout << "Texture " << textureName << " already loaded" << std::endl;
        return true;
    }

    // Crear nueva textura
    Texture newTexture;
    newTexture.name = textureName;

    // Cargar textura usando stb_image
    unsigned char* data = stbi_load(filePath.c_str(),
        &newTexture.width, &newTexture.height, &newTexture.channels, STBI_rgb_alpha);

    if (!data) {
        std::cerr << "Error loading texture: " << filePath << std::endl;
        return false;
    }

    // Generar y configurar la textura OpenGL
    glGenTextures(1, &newTexture.textureID);
    glBindTexture(GL_TEXTURE_2D, newTexture.textureID);

    // Parámetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Cargar la imagen en OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newTexture.width, newTexture.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    // Agregar la textura al mapa
    textures[textureName] = std::move(newTexture);
    std::cout << "Texture " << textureName << " loaded successfully" << std::endl;

    return true;
}

const Importer::Model* Importer::GetModel(const std::string& modelName) const {
    auto it = models.find(modelName);
    if (it != models.end()) {
        return &it->second;
    }
    return nullptr;
}

const std::string Importer::GetModelName(const std::string& filePath) const {
    std::string modelName = std::filesystem::path(filePath).stem().string();

    // Buscar el modelo en el mapa
    auto it = models.find(modelName);
    if (it != models.end()) {
        return it->second.name; 
    }
    return ""; 
}

const Importer::Texture* Importer::GetTexture(const std::string& textureName) const {
	auto it = textures.find(textureName);
	if (it != textures.end()) {
		return &it->second;
	}
	return nullptr;
}

const std::string Importer::GetTextureName(const std::string& filePath) const {
	std::string textureName = std::filesystem::path(filePath).stem().string();

	// Buscar la textura en el mapa
	auto it = textures.find(textureName);
	if (it != textures.end()) {
		return it->second.name;
	}
	return "";
}

