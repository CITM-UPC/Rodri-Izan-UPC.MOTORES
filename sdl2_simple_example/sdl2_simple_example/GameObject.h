#pragma once
#include <GL/glew.h>    
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

// Clase GameObject que define un objeto en la escena
class GameObject {

public:
    // Constructor
    GameObject(const std::string& name = "GameObject")
        : m_name(name),
        m_localPosition(0.0f, 0.0f, 0.0f),
        m_localRotation(0.0f, 0.0f, 0.0f),
        m_localScale(1.0f, 1.0f, 1.0f),
        m_active(true),
        m_parent(nullptr) {}

    // Destructor
    virtual ~GameObject() = default;

    void UpdateTransforms() {
        UpdateWorldMatrix();
        for (auto* child : m_children) {
            child->UpdateTransforms();
        }
    }

    // Getters
    const std::string& GetName() const { return m_name; }
    const glm::vec3& GetLocalPosition() const { return m_localPosition; }
    const glm::vec3& GetLocalRotation() const { return m_localRotation; }
    const glm::vec3& GetLocalScale() const { return m_localScale; }
    bool IsActive() const { return m_active; }

    // Setters
    void SetName(const std::string& name) { m_name = name; }

    void SetLocalPosition(const glm::vec3& position) {
        m_localPosition = position;
        UpdateWorldMatrix();
        // Actualizar los hijos
        for (auto* child : m_children) {
            child->UpdateTransforms();
        }
    }

    void SetLocalRotation(const glm::vec3& rotation) {
        m_localRotation = rotation;
        UpdateWorldMatrix();
        // Actualizar los hijos
        for (auto* child : m_children) {
            child->UpdateTransforms();
        }
    }

    void SetLocalScale(const glm::vec3& scale) {
        m_localScale = scale;
        UpdateWorldMatrix();
        // Actualizar los hijos
        for (auto* child : m_children) {
            child->UpdateTransforms();
        }
    }

    void SetActive(bool active) { m_active = active; }

    // Transformations
    void Translate(const glm::vec3& translation) {
        m_localPosition += translation;
        UpdateWorldMatrix();
        // Actualizar los hijos
        for (auto* child : m_children) {
            child->UpdateTransforms();
        }
    }

    void Rotate(const glm::vec3& rotation) {
        m_localRotation += rotation;
        UpdateWorldMatrix();
        // Actualizar los hijos
        for (auto* child : m_children) {
            child->UpdateTransforms();
        }
    }

    void Scale(const glm::vec3& scale) {
        m_localScale *= scale;
        UpdateWorldMatrix();
        // Actualizar los hijos
        for (auto* child : m_children) {
            child->UpdateTransforms();
        }
    }

    // Matriz de transformación
    glm::mat4 GetLocalTransformMatrix() const {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, m_localPosition);
        transform = glm::rotate(transform, glm::radians(m_localRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_localRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_localRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, m_localScale);
        return transform;
    }

    glm::vec3 GetGlobalPosition() const {
        return glm::vec3(GetGlobalTransformMatrix()[3]);
    }

    void SetGlobalPosition(const glm::vec3& globalPosition) {
        if (m_parent) {
            glm::mat4 inverseParentMatrix = glm::inverse(m_parent->GetGlobalTransformMatrix());
            glm::vec4 localPos = inverseParentMatrix * glm::vec4(globalPosition, 1.0f);
            m_localPosition = glm::vec3(localPos);
        }
        else {
            m_localPosition = globalPosition;
        }
    }

    glm::mat4 GetGlobalTransformMatrix() const {
        glm::mat4 transform = GetLocalTransformMatrix();
        if (m_parent) {
            transform = m_parent->GetGlobalTransformMatrix() * transform;
        }
        return transform;
    }


    void AddChild(GameObject* child) {
        if (!child || child == this) return;

        // Guardar la posición global actual del hijo
        glm::vec3 childGlobalPos = child->GetGlobalPosition();

        // Remover del padre anterior si existe
        if (child->m_parent) {
            child->m_parent->RemoveChild(child);
        }

        // Establecer la nueva relación padre-hijo
        m_children.push_back(child);
        child->m_parent = this;

        // Mantener la posición global ajustando la posición local
        child->SetGlobalPosition(childGlobalPos);
    }

    void RemoveChild(GameObject* child) {
        if (!child) return;

        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end()) {
            // Guardar la posición global antes de remover
            glm::vec3 globalPos = child->GetGlobalPosition();

            // Remover la relación padre-hijo
            m_children.erase(it);
            child->m_parent = nullptr;

            // Mantener la posición global
            child->SetGlobalPosition(globalPos);
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
        // Solo necesitamos actualizar los hijos
        for (GameObject* child : m_children) {
            child->UpdateTransform();
        }
    }

    const glm::vec3& GetPosition() const { return GetLocalPosition(); }
    const glm::vec3& GetRotation() const { return GetLocalRotation(); }
    const glm::vec3& GetScale() const { return GetLocalScale(); }

    void SetPosition(const glm::vec3& position) { SetLocalPosition(position); }
    void SetRotation(const glm::vec3& rotation) { SetLocalRotation(rotation); }
    void SetScale(const glm::vec3& scale) { SetLocalScale(scale); }


protected:
    // Atributos
    std::string m_name;
    glm::vec3 m_localPosition;    // Cambiar a localPosition para claridad
    glm::vec3 m_localRotation;    // Cambiar a localRotation
    glm::vec3 m_localScale;
    bool m_active;

    GameObject* m_parent = nullptr;                
    std::vector<GameObject*> m_children;

    void UpdateWorldMatrix() {
        m_worldMatrix = GetLocalTransformMatrix();
        if (m_parent) {
            m_worldMatrix = m_parent->m_worldMatrix * m_worldMatrix;
        }
    }

    glm::mat4 m_worldMatrix = glm::mat4(1.0f);
};



// Clase RenderableGameObject que se encarga de renderizar un objeto en la escena
class RenderableGameObject : public GameObject {
public:
    // Constructor
    RenderableGameObject(const std::string& name = "RenderableGameObject")
        : GameObject(name), m_textureID(0) {}
    
    // Añadir índice de malla
    void SetMeshIndex(int index) { 
        m_meshIndices.push_back(index);
    }

    // Añadir ID de textura
    void SetTextureID(GLuint textureID) { m_textureID = textureID; }

    // Obtener índices de malla
    const std::vector<int>& GetMeshIndices() const {
        return m_meshIndices;
    }

    // Obtener ID de textura
    GLuint GetTextureID() const {
        return m_textureID;
    }

    std::string GetModelName() const;
    std::string GetTextureName() const;



private:
    std::vector<int> m_meshIndices;   // Índice de la malla en el Importer
    GLuint m_textureID;   // ID de la textura
};