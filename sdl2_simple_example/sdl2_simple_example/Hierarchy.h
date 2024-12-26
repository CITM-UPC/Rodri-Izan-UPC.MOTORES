#pragma once
#include "GameObject.h"

// Clase que se encarga de mostrar la ventana de jerarquía
class Hierarchy {
public:
    Hierarchy();
    void DrawHierarchyWindow();
    static const char* GetDragDropType() { return "HIERARCHY_GAMEOBJECT"; }

private:
    void DrawGameObjectNode(GameObject* gameObject);
    int selectedGameObject;
};