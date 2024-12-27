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
    std::string customPath = "Library/Models/" + modelName + ".mdl";

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
        // Verificar que el número de vértices sea razonable
        if (aimesh->mNumVertices > 1000000) {
            std::cerr << "Too many vertices in mesh: " << aimesh->mNumVertices << std::endl;
            return false;
        }

        std::vector<GLfloat> vertices;
        std::vector<GLfloat> texCoords;
        std::vector<GLuint> indices;

        // Reservar espacio
        vertices.reserve(aimesh->mNumVertices * 3);
        texCoords.reserve(aimesh->mNumVertices * 2);

        // Procesar vértices y coordenadas de textura
        for (unsigned int i = 0; i < aimesh->mNumVertices; i++) {
            vertices.push_back(aimesh->mVertices[i].x);
            vertices.push_back(aimesh->mVertices[i].y);
            vertices.push_back(aimesh->mVertices[i].z);

            if (aimesh->HasTextureCoords(0)) {
                texCoords.push_back(aimesh->mTextureCoords[0][i].x);
                texCoords.push_back(aimesh->mTextureCoords[0][i].y);
            }
            else {
                texCoords.push_back(0.0f);
                texCoords.push_back(0.0f);
            }
        }

        // Verificar caras
        if (aimesh->mNumFaces > 1000000) {
            std::cerr << "Too many faces in mesh: " << aimesh->mNumFaces << std::endl;
            return false;
        }

        // Procesar índices
        indices.reserve(aimesh->mNumFaces * 3);
        for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
            const aiFace& face = aimesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Crear nueva malla usando la clase Mesh
        Mesh newMesh(vertices, texCoords, indices);
        model.meshes.push_back(std::move(newMesh));

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

    // Escribir nombre del modelo
    size_t nameLength = model.name.length();
    outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    outFile.write(model.name.c_str(), nameLength);

    // Escribir número de mallas
    size_t meshCount = model.meshes.size();
    outFile.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

    // Guardar cada malla
    for (size_t i = 0; i < model.meshes.size(); ++i) {
        const auto& mesh = model.meshes[i];

        std::string meshFileName = libraryPath + "Meshes/" + modelName + "_Mesh" + std::to_string(i) + ".msh";
        std::ofstream meshFile(meshFileName, std::ios::binary);

        if (!meshFile.is_open()) {
            std::cerr << "Error opening mesh file for saving: " << meshFileName << std::endl;
            return false;
        }

        // Guardar vértices
        size_t vertexCount = mesh.vertices.size();
        meshFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
        meshFile.write(reinterpret_cast<const char*>(mesh.vertices.data()), vertexCount * sizeof(GLfloat));

        // Guardar coordenadas de textura
        size_t texCoordCount = mesh.texCoords.size();
        meshFile.write(reinterpret_cast<const char*>(&texCoordCount), sizeof(texCoordCount));
        meshFile.write(reinterpret_cast<const char*>(mesh.texCoords.data()), texCoordCount * sizeof(GLfloat));

        // Guardar índices
        size_t indexCount = mesh.indices.size();
        meshFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
        meshFile.write(reinterpret_cast<const char*>(mesh.indices.data()), indexCount * sizeof(GLuint));

        // Escribir nombre del archivo de malla
        size_t meshFileNameLength = meshFileName.length();
        outFile.write(reinterpret_cast<const char*>(&meshFileNameLength), sizeof(meshFileNameLength));
        outFile.write(meshFileName.c_str(), meshFileNameLength);

        meshFile.close();
    }

    return true;
}

bool Importer::LoadModelFromCustomFormat(const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error opening file for loading: " << filePath << std::endl;
        return false;
    }

    // Leer nombre del modelo
    size_t nameLength;
    inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    std::string modelName(nameLength, '\0');
    inFile.read(&modelName[0], nameLength);

    Model newModel;
    newModel.name = modelName;

    // Leer número de mallas
    size_t meshCount;
    inFile.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));

    // Cargar cada malla
    for (size_t i = 0; i < meshCount; ++i) {
        size_t meshFileNameLength;
        inFile.read(reinterpret_cast<char*>(&meshFileNameLength), sizeof(meshFileNameLength));
        std::string meshFileName(meshFileNameLength, '\0');
        inFile.read(&meshFileName[0], meshFileNameLength);

        std::ifstream meshFile(meshFileName, std::ios::binary);
        if (!meshFile.is_open()) {
            std::cerr << "Error opening mesh file for loading: " << meshFileName << std::endl;
            return false;
        }

        std::vector<GLfloat> vertices;
        std::vector<GLfloat> texCoords;
        std::vector<GLuint> indices;
        size_t vertexCount, texCoordCount, indexCount;

        // Leer vértices
        meshFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
        vertices.resize(vertexCount);
        meshFile.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(GLfloat));

        // Leer coordenadas de textura
        meshFile.read(reinterpret_cast<char*>(&texCoordCount), sizeof(texCoordCount));
        texCoords.resize(texCoordCount);
        meshFile.read(reinterpret_cast<char*>(texCoords.data()), texCoordCount * sizeof(GLfloat));

        // Leer índices
        meshFile.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
        indices.resize(indexCount);
        meshFile.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(GLuint));

        // Crear nueva malla con los datos cargados
        Mesh newMesh(vertices, texCoords, indices);
        newModel.meshes.push_back(std::move(newMesh));
    }

    // Agregar el modelo al mapa
    models[modelName] = std::move(newModel);

    return true;
}

bool Importer::ImportTexture(const std::string& filePath) {
    namespace fs = std::filesystem;
    std::string textureName = fs::path(filePath).stem().string();
    std::string customPath = libraryPath + "Materials/" + textureName + ".tex";

    // Verificar si la textura ya está cargada
    auto it = textures.find(textureName);
    if (it != textures.end()) {
        std::cout << "Texture " << textureName << " already loaded" << std::endl;
        return true;
    }

    // Intentar cargar el archivo custom primero
    if (fs::exists(customPath)) {
        std::cout << "Custom texture format found, loading " << customPath << std::endl;
        return LoadTextureFromCustomFormat(customPath);
    }

    // Crear nueva textura
    Texture newTexture;
    newTexture.name = textureName;

    try {
        // Cargar textura usando stb_image
        unsigned char* data = stbi_load(filePath.c_str(),
            &newTexture.width, &newTexture.height, &newTexture.channels, STBI_rgb_alpha);

        if (!data) {
            std::cerr << "Error loading texture: " << filePath << std::endl;
            return false;
        }

        // Verificar dimensiones válidas
        const int MAX_TEXTURE_SIZE = 16384;
        if (newTexture.width <= 0 || newTexture.height <= 0 ||
            newTexture.width > MAX_TEXTURE_SIZE || newTexture.height > MAX_TEXTURE_SIZE) {
            std::cerr << "Invalid texture dimensions: " << newTexture.width << "x" << newTexture.height << std::endl;
            stbi_image_free(data);
            return false;
        }

        // Verificar tamaño total
        size_t dataSize = static_cast<size_t>(newTexture.width) *
            static_cast<size_t>(newTexture.height) * 4;
        if (dataSize > std::numeric_limits<size_t>::max() / 4) {
            std::cerr << "Texture size too large" << std::endl;
            stbi_image_free(data);
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

        // Verificar errores de OpenGL
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error while loading texture: " << error << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &newTexture.textureID);
            return false;
        }

        // Guardar en formato custom antes de liberar los datos
        if (!SaveTextureToCustomFormat(textureName, data, newTexture)) {
            std::cerr << "Failed to save texture in custom format" << std::endl;
        }

        stbi_image_free(data);

        // Agregar la textura al mapa
        textures[textureName] = std::move(newTexture);
        std::cout << "Texture " << textureName << " loaded successfully" << std::endl;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while loading texture: " << e.what() << std::endl;
        return false;
    }
}

bool Importer::SaveTextureToCustomFormat(const std::string& textureName, unsigned char* data, const Texture& texture) {
    try {
        std::string outputPath = libraryPath + "Materials/" + textureName + ".tex";
        std::ofstream outFile(outputPath, std::ios::binary);

        if (!outFile.is_open()) {
            std::cerr << "Error opening file for saving texture: " << outputPath << std::endl;
            return false;
        }

        // Verificar nombre válido
        if (texture.name.empty() || texture.name.length() > 1024) {
            std::cerr << "Invalid texture name length" << std::endl;
            return false;
        }

        // Escribir el nombre de la textura
        size_t nameLength = texture.name.length();
        outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        outFile.write(texture.name.c_str(), nameLength);

        // Escribir las dimensiones y canales
        outFile.write(reinterpret_cast<const char*>(&texture.width), sizeof(texture.width));
        outFile.write(reinterpret_cast<const char*>(&texture.height), sizeof(texture.height));
        outFile.write(reinterpret_cast<const char*>(&texture.channels), sizeof(texture.channels));

        // Verificar y escribir los datos de la imagen
        size_t dataSize = static_cast<size_t>(texture.width) *
            static_cast<size_t>(texture.height) * 4;
        if (dataSize == 0 || dataSize > std::numeric_limits<size_t>::max() / 4) {
            std::cerr << "Invalid texture data size" << std::endl;
            return false;
        }

        outFile.write(reinterpret_cast<const char*>(data), dataSize);

        if (!outFile) {
            std::cerr << "Error writing texture data" << std::endl;
            return false;
        }

        std::cout << "Texture " << textureName << " saved successfully to " << outputPath << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while saving texture: " << e.what() << std::endl;
        return false;
    }
}

bool Importer::LoadTextureFromCustomFormat(const std::string& filePath) {
    try {
        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile.is_open()) {
            std::cerr << "Error opening file for loading texture: " << filePath << std::endl;
            return false;
        }

        // Leer el nombre de la textura
        size_t nameLength;
        inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

        // Verificar longitud del nombre
        if (nameLength == 0 || nameLength > 1024) {
            std::cerr << "Invalid texture name length" << std::endl;
            return false;
        }

        std::string textureName(nameLength, '\0');
        inFile.read(&textureName[0], nameLength);

        // Crear nueva textura
        Texture newTexture;
        newTexture.name = textureName;

        // Leer dimensiones y canales
        inFile.read(reinterpret_cast<char*>(&newTexture.width), sizeof(newTexture.width));
        inFile.read(reinterpret_cast<char*>(&newTexture.height), sizeof(newTexture.height));
        inFile.read(reinterpret_cast<char*>(&newTexture.channels), sizeof(newTexture.channels));

        // Verificar dimensiones válidas
        const int MAX_TEXTURE_SIZE = 16384;
        if (newTexture.width <= 0 || newTexture.height <= 0 ||
            newTexture.width > MAX_TEXTURE_SIZE || newTexture.height > MAX_TEXTURE_SIZE) {
            std::cerr << "Invalid texture dimensions: " << newTexture.width << "x" << newTexture.height << std::endl;
            return false;
        }

        // Verificar tamaño total
        size_t dataSize = static_cast<size_t>(newTexture.width) *
            static_cast<size_t>(newTexture.height) * 4;
        if (dataSize > std::numeric_limits<size_t>::max() / 4) {
            std::cerr << "Texture size too large" << std::endl;
            return false;
        }

        // Leer los datos de la imagen
        std::vector<unsigned char> data(dataSize);
        inFile.read(reinterpret_cast<char*>(data.data()), dataSize);

        if (!inFile) {
            std::cerr << "Error reading texture data" << std::endl;
            return false;
        }

        // Generar y configurar la textura OpenGL
        glGenTextures(1, &newTexture.textureID);
        glBindTexture(GL_TEXTURE_2D, newTexture.textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newTexture.width, newTexture.height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

        // Verificar errores de OpenGL
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error while loading texture: " << error << std::endl;
            glDeleteTextures(1, &newTexture.textureID);
            return false;
        }

        // Agregar la textura al mapa
        textures[textureName] = std::move(newTexture);
        std::cout << "Texture " << textureName << " loaded successfully from custom format" << std::endl;

        return true;
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading texture: " << e.what() << std::endl;
        return false;
    }
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

