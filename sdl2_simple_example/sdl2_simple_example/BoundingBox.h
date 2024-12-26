#pragma once
#include <glm/glm.hpp>
#include <vector>

class BoundingBox {
public:
    BoundingBox() :
        minPoint(std::numeric_limits<float>::max()),
        maxPoint(-std::numeric_limits<float>::max()) {
    }

    // Establece los puntos mínimo y máximo
    void SetMinMax(const glm::vec3& min, const glm::vec3& max) {
        minPoint = min;
        maxPoint = max;
    }

    // Establece la caja a "infinito negativo"
    void SetNegativeInfinity() {
        minPoint = glm::vec3(std::numeric_limits<float>::max());
        maxPoint = glm::vec3(-std::numeric_limits<float>::max());
    }

    // Expande la caja para incluir un punto
    void Encapsulate(const glm::vec3& point) {
        minPoint = glm::min(minPoint, point);
        maxPoint = glm::max(maxPoint, point);
    }

    // Expande la caja para incluir otra caja
    void Enclose(const BoundingBox& other) {
        minPoint = glm::min(minPoint, other.minPoint);
        maxPoint = glm::max(maxPoint, other.maxPoint);
    }

    // Transforma la caja usando una matriz (útil para OBB)
    void Transform(const glm::mat4& matrix) {
        std::vector<glm::vec3> corners = GetCorners();
        SetNegativeInfinity();

        for (const auto& corner : corners) {
            glm::vec4 transformedCorner = matrix * glm::vec4(corner, 1.0f);
            Encapsulate(glm::vec3(transformedCorner));
        }
    }

    // Obtiene los 8 vértices de la caja
    std::vector<glm::vec3> GetCorners() const {
        std::vector<glm::vec3> corners;
        corners.reserve(8);

        corners.push_back(glm::vec3(minPoint.x, minPoint.y, minPoint.z));
        corners.push_back(glm::vec3(maxPoint.x, minPoint.y, minPoint.z));
        corners.push_back(glm::vec3(minPoint.x, maxPoint.y, minPoint.z));
        corners.push_back(glm::vec3(maxPoint.x, maxPoint.y, minPoint.z));
        corners.push_back(glm::vec3(minPoint.x, minPoint.y, maxPoint.z));
        corners.push_back(glm::vec3(maxPoint.x, minPoint.y, maxPoint.z));
        corners.push_back(glm::vec3(minPoint.x, maxPoint.y, maxPoint.z));
        corners.push_back(glm::vec3(maxPoint.x, maxPoint.y, maxPoint.z));

        return corners;
    }

    // Getters
    const glm::vec3& GetMin() const { return minPoint; }
    const glm::vec3& GetMax() const { return maxPoint; }
    glm::vec3 GetCenter() const { return (minPoint + maxPoint) * 0.5f; }
    glm::vec3 GetSize() const { return maxPoint - minPoint; }

private:
    glm::vec3 minPoint;
    glm::vec3 maxPoint;
};