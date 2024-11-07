#pragma once
#include "Importer.h"
#include "MyWindow.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Biblio.h"

class EditScene {
public:
    EditScene();
    void RenderEditorWindows(MyWindow& window, Importer* importer, void(*renderSceneContent)(MyWindow&, Importer*));
    void RenderSceneWindow(MyWindow& window, Importer* importer, void(*renderSceneContent)(MyWindow&, Importer*));
    void RenderInspectorWindow();
    void RenderHierarchyWindow();
    void RenderAssetsWindow();

private:
    Hierarchy hierarchy;
    Inspector inspector;
    Biblio assets;
};