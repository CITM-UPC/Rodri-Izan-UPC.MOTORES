// Transform.h
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
    Transform() :
        position(0.0f),
        rotation(0.0f),
        scale(1.0f) {
        UpdateMatrix();
    }

    void SetPosition(const glm::vec3& pos) {
        position = pos;
        UpdateMatrix();
    }

    void SetRotation(const glm::vec3& rot) {
        rotation = rot;
        UpdateMatrix();
    }

    void SetScale(const glm::vec3& sc) {
        scale = sc;
        UpdateMatrix();
    }

    const glm::vec3& GetPosition() const { return position; }
    const glm::vec3& GetRotation() const { return rotation; }
    const glm::vec3& GetScale() const { return scale; }
    const glm::mat4& GetMatrix() const { return modelMatrix; }

private:
    void UpdateMatrix() {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, scale);
    }

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 modelMatrix;
};