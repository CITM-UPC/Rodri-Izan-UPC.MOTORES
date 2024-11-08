#pragma once
#include <GL/glew.h>    
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

class GameObject {
public:
    GameObject(const std::string& name = "GameObject")
        : m_name(name),
        m_position(0.0f, 0.0f, 0.0f),
        m_rotation(0.0f, 0.0f, 0.0f),
        m_scale(1.0f, 1.0f, 1.0f),
        m_active(true) {}

    // Añadir destructor virtual para permitir polimorfismo
    virtual ~GameObject() = default;

    // Resto de la clase permanece igual...
    const std::string& GetName() const { return m_name; }
    const glm::vec3& GetPosition() const { return m_position; }
    const glm::vec3& GetRotation() const { return m_rotation; }
    const glm::vec3& GetScale() const { return m_scale; }
    bool IsActive() const { return m_active; }

    void SetName(const std::string& name) { m_name = name; }
    void SetPosition(const glm::vec3& position) { m_position = position; }
    void SetRotation(const glm::vec3& rotation) { m_rotation = rotation; }
    void SetScale(const glm::vec3& scale) { m_scale = scale; }
    void SetActive(bool active) { m_active = active; }

    void Translate(const glm::vec3& translation) { m_position += translation; }
    void Rotate(const glm::vec3& rotation) { m_rotation += rotation; }
    void Scale(const glm::vec3& scale) { m_scale *= scale; }

    glm::mat4 GetTransformMatrix() const {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, m_position);
        transform = glm::rotate(transform, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, m_scale);
        return transform;
    }

protected:
    std::string m_name;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    bool m_active;
};

class RenderableGameObject : public GameObject {
public:
    RenderableGameObject(const std::string& name = "RenderableGameObject")
        : GameObject(name), m_textureID(0) {}

    void SetMeshIndex(int index) {
        m_meshIndices.push_back(index);
    }
    void SetTextureID(GLuint textureID) { m_textureID = textureID; }

    const std::vector<int>& GetMeshIndices() const {
        return m_meshIndices;
    }

    GLuint GetTextureID() const {
        return m_textureID;
    }

private:
    std::vector<int> m_meshIndices;   // Índice de la malla en el Importer
    GLuint m_textureID;   // ID de la textura
};