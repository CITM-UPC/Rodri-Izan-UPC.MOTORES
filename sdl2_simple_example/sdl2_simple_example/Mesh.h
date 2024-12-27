#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "BoundingBox.h"

// Clase para almacenar y renderizar mallas
class Mesh {
public:
    //Constructor
    Mesh() = default; 

    //Constructor con datos de malla
    Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLfloat>& texCoords,
        const std::vector<GLuint>& indices)
        : vertices(vertices), texCoords(texCoords), indices(indices) {
        SetupMesh();
    }

    //Destructor
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

	BoundingBox boundingBox; // Caja de colisión

	// Configurar los datos de malla en OpenGL
    void SetupMesh() {
        // Verifica si hay datos de vértices e índices antes de configurar el mesh
        if (!vertices.empty() && !indices.empty()) {

            // Genera y enlaza el VAO 
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            // Genera y enlaza el VBO 
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

            // Configura el atributo de posición en el índice 0
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
            glEnableVertexAttribArray(0);

            // Configura opcionalmente las coordenadas de textura solo si están disponibles
            if (!texCoords.empty()) {
                GLuint texCoordVBO;
                glGenBuffers(1, &texCoordVBO);
                glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
                glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(GLfloat), texCoords.data(), GL_STATIC_DRAW);

                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
                glEnableVertexAttribArray(1);
            }

            // Genera y enlaza el EBO
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

            // Desvincula el VAO 
            glBindVertexArray(0);

            boundingBox.CalculateFromVertices(vertices);
        }
    }

    const BoundingBox& GetBoundingBox() const { return boundingBox; }

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
};