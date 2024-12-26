#pragma once
#include <GL/glew.h>    
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "BoundingBox.h"
#include "Mesh.h"

// Clase GameObject que define un objeto en la escena
class GameObject {
public:
    // Constructor
    GameObject(const std::string& name = "GameObject")
        : m_name(name),
        m_position(0.0f, 0.0f, 0.0f),
        m_rotation(0.0f, 0.0f, 0.0f),
        m_scale(1.0f, 1.0f, 1.0f),
        m_active(true) {}

    // Destructor
    virtual ~GameObject() = default;

    // Getters
    const std::string& GetName() const { return m_name; }
    const glm::vec3& GetPosition() const { return m_position; }
    const glm::vec3& GetRotation() const { return m_rotation; }
    const glm::vec3& GetScale() const { return m_scale; }
    bool IsActive() const { return m_active; }

    // Setters
    void SetName(const std::string& name) { m_name = name; }
    void SetPosition(const glm::vec3& position) { m_position = position; }
    void SetRotation(const glm::vec3& rotation) { m_rotation = rotation; }
    void SetScale(const glm::vec3& scale) { m_scale = scale; }
    void SetActive(bool active) { m_active = active; }

    // Transformations
    void Translate(const glm::vec3& translation) { m_position += translation; }
    void Rotate(const glm::vec3& rotation) { m_rotation += rotation; }
    void Scale(const glm::vec3& scale) { m_scale *= scale; }

    // Matriz de transformación
    glm::mat4 GetTransformMatrix() const {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, m_position);
        transform = glm::rotate(transform, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, m_scale);
        return transform;
    }

    glm::vec3 GetGlobalPosition() const {
        glm::vec3 globalPos = m_position;
        if (m_parent) {
            globalPos = glm::vec3(m_parent->GetGlobalTransformMatrix() * glm::vec4(m_position, 1.0f));
        }
        return globalPos;
    }

    glm::mat4 GetGlobalTransformMatrix() const {
        glm::mat4 transform = GetTransformMatrix();
        if (m_parent) {
            transform = m_parent->GetGlobalTransformMatrix() * transform;
        }
        return transform;
    }


    void AddChild(GameObject* child) {
        if (!child || child == this) return;

        // Si el child ya tiene un padre, primero lo removemos
        if (GameObject* oldParent = child->GetParent()) {
            oldParent->RemoveChild(child);
        }

        m_children.push_back(child);
        child->SetParent(this);
    }

    void RemoveChild(GameObject* child) {
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end()) {
            (*it)->SetParent(nullptr);
            m_children.erase(it);
        }
    }

    GameObject* GetParent() const {
        return m_parent;
    }

    const std::vector<GameObject*>& GetChildren() const {
        return m_children;
    }

    void SetParent(GameObject* parent) {
        m_parent = parent;
    }

    void UpdateTransform() {
        if (m_parent) {
            m_position += m_parent->GetPosition(); // Simplificación: hereda solo posición
            // Agrega lógica adicional para rotación y escala si es necesario
        }
        for (GameObject* child : m_children) {
            child->UpdateTransform();
        }
    }


protected:
    // Atributos
    std::string m_name;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    bool m_active;

    GameObject* m_parent = nullptr;                
    std::vector<GameObject*> m_children;
};

// Clase RenderableGameObject que se encarga de renderizar un objeto en la escena
class RenderableGameObject : public GameObject {
public:
    RenderableGameObject(const std::string& name = "RenderableGameObject")
        : GameObject(name), m_textureID(0) {
    }

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

    // Nuevos métodos para bounding boxes
    void UpdateBoundingBoxes(const Mesh& mesh) {
        localAABB = mesh.GetLocalAABB();
        globalAABB = localAABB;
        globalAABB.Transform(GetGlobalTransformMatrix());
    }

    BoundingBox localAABB;
    BoundingBox globalAABB;

private:
    std::vector<int> m_meshIndices;
    GLuint m_textureID;
};