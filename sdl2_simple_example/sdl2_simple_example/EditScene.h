#pragma once
#include "Importer.h"
#include "MyWindow.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Biblio.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "Performance.h"

// Clase que se encarga de motrar las ventanas de la escena
class EditScene {
public:
    EditScene();
    void RenderEditorWindows(MyWindow& window, Importer* importer,
        void(*renderSceneContent)(MyWindow&, Importer*));

    void RenderMenuBar();

    void RenderSceneWindow(MyWindow& window, Importer* importer,
        void(*renderSceneContent)(MyWindow&, Importer*));

    void RenderInspectorWindow();
    void RenderHierarchyWindow();
    void RenderAssetsWindow();
    void RenderPerformanceWindow();

private:
    Hierarchy hierarchy;
    Inspector inspector;
    Biblio biblio;
    PerformanceWindow performance;
};