// GameObjectManager.h
#pragma once
#include "GameObject.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

class GameObjectManager {
public:
    static GameObjectManager& GetInstance() {
        static GameObjectManager instance;
        return instance;
    }

    // Prohibir copia y asignación del singleton
    GameObjectManager(const GameObjectManager&) = delete;
    GameObjectManager& operator=(const GameObjectManager&) = delete;

    // Métodos para gestionar GameObjects
    template<typename T = GameObject>
    T* CreateGameObject(const std::string& name = "") {
        std::string objectName = name.empty() ? "GameObject_" + std::to_string(m_nextId) : name;
        auto gameObject = std::make_unique<T>(objectName);
        T* rawPtr = gameObject.get();

        m_gameObjects[m_nextId] = std::move(gameObject);
        m_objectIds[objectName] = m_nextId;
        m_nextId++;

        return rawPtr;
    }

    void DestroyGameObject(const std::string& name) {
        auto idIt = m_objectIds.find(name);
        if (idIt != m_objectIds.end()) {
            m_gameObjects.erase(idIt->second);
            m_objectIds.erase(idIt);
        }
    }

    void DestroyGameObject(size_t id) {
        auto objIt = m_gameObjects.find(id);
        if (objIt != m_gameObjects.end()) {
            std::string name = objIt->second->GetName();
            m_objectIds.erase(name);
            m_gameObjects.erase(objIt);
        }
    }

    GameObject* FindGameObject(const std::string& name) {
        auto idIt = m_objectIds.find(name);
        if (idIt != m_objectIds.end()) {
            return m_gameObjects[idIt->second].get();
        }
        return nullptr;
    }

    GameObject* GetGameObject(size_t id) {
        auto it = m_gameObjects.find(id);
        return it != m_gameObjects.end() ? it->second.get() : nullptr;
    }

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

    std::vector<GameObject*> GetAllGameObjects() {
        std::vector<GameObject*> objects;
        objects.reserve(m_gameObjects.size());
        for (const auto& pair : m_gameObjects) {
            objects.push_back(pair.second.get());
        }
        return objects;
    }

    void Clear() {
        m_gameObjects.clear();
        m_objectIds.clear();
        m_nextId = 0;
    }

    GameObject* GetSelectedGameObject() {
        return m_selectedGameObject;
    }

    void SetSelectedGameObject(GameObject* gameObject) {
        m_selectedGameObject = gameObject;
    }

private:
    GameObjectManager() : m_nextId(0) {}
    GameObject* m_selectedGameObject = nullptr;
    std::unordered_map<size_t, std::unique_ptr<GameObject>> m_gameObjects;
    std::unordered_map<std::string, size_t> m_objectIds;
    size_t m_nextId;
};