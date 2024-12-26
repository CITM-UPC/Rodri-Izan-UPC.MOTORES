#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "BoundingBox.h"

class Mesh {
public:
    // Constructor por defecto
    Mesh() = default;

    // Constructor único con datos de malla
    Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLfloat>& texCoords,
        const std::vector<GLuint>& indices)
        : vertices(vertices), texCoords(texCoords), indices(indices) {
        CalculateLocalAABB();
        SetupMesh();
    }

    // Destructor
    ~Mesh() {
        Cleanup();
    }

    // Datos de malla
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> texCoords;
    std::vector<GLfloat> normals;
    std::vector<GLuint> indices;

    // Identificadores de OpenGL
    GLuint VAO = 0;  // Vertex Array Object
    GLuint VBO = 0;  // Vertex Buffer Object
    GLuint EBO = 0;  // Element Buffer Object

    // Configurar los datos de malla en OpenGL
    void SetupMesh() {
        if (!vertices.empty() && !indices.empty()) {
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
            glEnableVertexAttribArray(0);

            if (!texCoords.empty()) {
                GLuint texCoordVBO;
                glGenBuffers(1, &texCoordVBO);
                glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
                glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(GLfloat), texCoords.data(), GL_STATIC_DRAW);

                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
                glEnableVertexAttribArray(1);
            }

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

            glBindVertexArray(0);
        }
    }

    // Limpiar los identificadores de OpenGL
    void Cleanup() {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        if (EBO != 0) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }
    }

    // AABB
    BoundingBox localAABB;

    // Método para calcular la AABB local
    void CalculateLocalAABB() {
        if (vertices.empty()) return;

        localAABB.SetNegativeInfinity();

        for (size_t i = 0; i < vertices.size(); i += 3) {
            glm::vec3 vertex(
                vertices[i],
                vertices[i + 1],
                vertices[i + 2]
            );
            localAABB.Encapsulate(vertex);
        }
    }

    // Getter para la AABB local
    BoundingBox GetLocalAABB() const {
        return localAABB;
    }
};