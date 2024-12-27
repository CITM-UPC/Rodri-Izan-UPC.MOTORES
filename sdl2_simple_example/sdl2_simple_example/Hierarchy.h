#pragma once
#include "GameObject.h"

// Clase que se encarga de mostrar la ventana de jerarquía
class Hierarchy {
public:
    Hierarchy();
    void DrawHierarchyWindow();

    void DrawGameObjectNode(GameObject* gameObject);

private:
    int selectedGameObject;
};