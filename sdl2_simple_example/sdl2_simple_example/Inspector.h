#pragma once

// Clase que se encarga de mostrar la ventana de inspector
class Inspector {
public:
    Inspector();
    void DrawInspectorWindow();

private:
    int selectedGameObject;
    bool isActive;
};