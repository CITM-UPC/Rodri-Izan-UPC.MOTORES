#pragma once
#include "Importer.h"
#include "MyWindow.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Transform.h"

class EditScene {
public:
    EditScene();
    void RenderSceneWindow(MyWindow& window, Importer* importer);
    void RenderInspectorWindow();
    void RenderHierarchyWindow();
    void RenderAssetsWindow();

private:
    Hierarchy hierarchy;
    Inspector inspector;
    Transform assets;
};