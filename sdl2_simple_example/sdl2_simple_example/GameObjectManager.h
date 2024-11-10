#pragma once
#include "GameObject.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

// Clase singleton que se encarga de gestionar los GameObjects
class GameObjectManager {
public:
    // Método para obtener la instancia del singleton
    static GameObjectManager& GetInstance(){
        static GameObjectManager instance;
        return instance;
    }

    // Prohibir copia y asignación del singleton
    GameObjectManager(const GameObjectManager&) = delete;
    GameObjectManager& operator=(const GameObjectManager&) = delete;

    // Métodos para gestionar GameObjects
    template<typename T = GameObject>
    T* CreateGameObject(const std::string& name = "") {
        std::string objectName = name.empty() ? "GameObject_" + std::to_string(m_nextId) : name; // Nombre por defecto
        auto gameObject = std::make_unique<T>(objectName); // Crear el GameObject
        T* rawPtr = gameObject.get(); 
        m_gameObjects[m_nextId] = std::move(gameObject); // Agregar el GameObject al mapa
        m_objectIds[objectName] = m_nextId; // Agregar el ID del GameObject al mapa
        m_nextId++; // Incrementar el ID

        return rawPtr;
    }

    // Eliminar un GameObject por nombre
    void DestroyGameObject(const std::string& name) {
        auto idIt = m_objectIds.find(name); // Buscar el ID del GameObject
        if (idIt != m_objectIds.end()) { 
            m_gameObjects.erase(idIt->second); // Eliminar el GameObject
            m_objectIds.erase(idIt); // Eliminar el ID del GameObject
        }
    }

    // Eliminar un GameObject por ID
    void DestroyGameObject(size_t id) {
        auto objIt = m_gameObjects.find(id); // Buscar el GameObject
        if (objIt != m_gameObjects.end()) { 
            std::string name = objIt->second->GetName(); // Obtener el nombre del GameObject
            m_objectIds.erase(name); // Eliminar el ID del GameObject
            m_gameObjects.erase(objIt); // Eliminar el GameObject
        }
    }

    // Buscar un GameObject por nombre
    GameObject* FindGameObject(const std::string& name) {
        auto idIt = m_objectIds.find(name);
        if (idIt != m_objectIds.end()) {
            return m_gameObjects[idIt->second].get();
        }
        return nullptr;
    }

    // Buscar un GameObject por ID
    GameObject* GetGameObject(size_t id) {
        auto it = m_gameObjects.find(id);
        return it != m_gameObjects.end() ? it->second.get() : nullptr;
    }

    // Obtener todos los GameObjects de un tipo concreto, esto es para asignarle el tipo RenderableGameObject
    template<typename T>
    std::vector<T*> GetGameObjectsOfType() {
        std::vector<T*> objects;
        for (const auto& pair : m_gameObjects) {
            if (T* obj = dynamic_cast<T*>(pair.second.get())) {
                objects.push_back(obj);
            }
        }
        return objects;
    }

    // Obtener todos los GameObjects
    std::vector<GameObject*> GetAllGameObjects() {
        std::vector<GameObject*> objects;
        objects.reserve(m_gameObjects.size());
        for (const auto& pair : m_gameObjects) {
            objects.push_back(pair.second.get());
        }
        return objects;
    }

    // Limpiar todos los GameObjects
    void Clear() {
        m_gameObjects.clear();
        m_objectIds.clear();
        m_nextId = 0;
    }

    // Obtener el GameObject seleccionado
    GameObject* GetSelectedGameObject() {
        return m_selectedGameObject;
    }

    // Establecer el GameObject seleccionado
    void SetSelectedGameObject(GameObject* gameObject) {
        m_selectedGameObject = gameObject;
    }

private:
    GameObjectManager() : m_nextId(0) {} // Constructor privado para el singleton
    GameObject* m_selectedGameObject = nullptr; // GameObject seleccionado
    std::unordered_map<size_t, std::unique_ptr<GameObject>> m_gameObjects; // Mapa de GameObjects
    std::unordered_map<std::string, size_t> m_objectIds; // Mapa de IDs de GameObjects
    size_t m_nextId; // ID del siguiente GameObject
};