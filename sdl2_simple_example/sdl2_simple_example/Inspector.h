#pragma once

class Inspector {
public:
    Inspector();
    void DrawInspectorWindow();

private:
    int selectedGameObject;
    bool isActive;
};