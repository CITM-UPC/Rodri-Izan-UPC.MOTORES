#pragma once
#include <glm/glm.hpp>
#include <vector>

class BoundingBox {
public:
    BoundingBox() : min(0.0f), max(0.0f), center(0.0f), extents(0.0f) {}

    // Constructor que calcula la BoundingBox a partir de vértices
    BoundingBox(const std::vector<GLfloat>& vertices) {
        CalculateFromVertices(vertices);
    }

    // Calcula la BoundingBox desde los vértices
    void CalculateFromVertices(const std::vector<GLfloat>& vertices) {
        if (vertices.empty()) return;

        min = max = glm::vec3(vertices[0], vertices[1], vertices[2]);

        for (size_t i = 0; i < vertices.size(); i += 3) {
            glm::vec3 vertex(vertices[i], vertices[i + 1], vertices[i + 2]);
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }

        UpdateDerivedData();
    }

    // Actualiza el centro y las extensiones
    void UpdateDerivedData() {
        center = (min + max) * 0.5f;
        extents = (max - min) * 0.5f;
    }

    // Transforma la BoundingBox según una matriz
    void Transform(const glm::mat4& transform) {
        std::vector<glm::vec3> corners = GetCorners();
        min = glm::vec3(std::numeric_limits<float>::max());
        max = glm::vec3(std::numeric_limits<float>::lowest());

        for (const auto& corner : corners) {
            glm::vec4 transformedCorner = transform * glm::vec4(corner, 1.0f);
            glm::vec3 transformedPoint(transformedCorner);

            min = glm::min(min, transformedPoint);
            max = glm::max(max, transformedPoint);
        }

        UpdateDerivedData();
    }

    // Obtiene los 8 vértices de la caja
    std::vector<glm::vec3> GetCorners() const {
        std::vector<glm::vec3> corners;
        corners.reserve(8);

        corners.push_back(glm::vec3(min.x, min.y, min.z));
        corners.push_back(glm::vec3(max.x, min.y, min.z));
        corners.push_back(glm::vec3(min.x, max.y, min.z));
        corners.push_back(glm::vec3(max.x, max.y, min.z));
        corners.push_back(glm::vec3(min.x, min.y, max.z));
        corners.push_back(glm::vec3(max.x, min.y, max.z));
        corners.push_back(glm::vec3(min.x, max.y, max.z));
        corners.push_back(glm::vec3(max.x, max.y, max.z));

        return corners;
    }

    // Detecta colisión con otra BoundingBox
    bool Intersects(const BoundingBox& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
            (min.y <= other.max.y && max.y >= other.min.y) &&
            (min.z <= other.max.z && max.z >= other.min.z);
    }

    // Dibuja la BoundingBox (útil para debug)
    void Draw() const {
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.0f, 1.0f, 0.0f); // Verde para debug

        auto corners = GetCorners();

        glBegin(GL_LINES);
        // Front face
        glVertex3fv(&corners[0].x); glVertex3fv(&corners[1].x);
        glVertex3fv(&corners[1].x); glVertex3fv(&corners[3].x);
        glVertex3fv(&corners[3].x); glVertex3fv(&corners[2].x);
        glVertex3fv(&corners[2].x); glVertex3fv(&corners[0].x);

        // Back face
        glVertex3fv(&corners[4].x); glVertex3fv(&corners[5].x);
        glVertex3fv(&corners[5].x); glVertex3fv(&corners[7].x);
        glVertex3fv(&corners[7].x); glVertex3fv(&corners[6].x);
        glVertex3fv(&corners[6].x); glVertex3fv(&corners[4].x);

        // Connecting lines
        glVertex3fv(&corners[0].x); glVertex3fv(&corners[4].x);
        glVertex3fv(&corners[1].x); glVertex3fv(&corners[5].x);
        glVertex3fv(&corners[2].x); glVertex3fv(&corners[6].x);
        glVertex3fv(&corners[3].x); glVertex3fv(&corners[7].x);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f); // Restaurar color
        glEnable(GL_TEXTURE_2D);
    }

    glm::vec3 min;      // Punto mínimo
    glm::vec3 max;      // Punto máximo
    glm::vec3 center;   // Centro de la caja
    glm::vec3 extents;  // Mitad del tamaño en cada eje
};