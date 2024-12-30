#pragma once
#include "Importer.h"
#include "Console.h"
#include "MyWindow.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Biblio.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "Performance.h"
#include "Resources.h"

// Clase que se encarga de motrar las ventanas de la escena
class EditScene {
public:
    EditScene();
    void RenderEditorWindows(MyWindow& window, Importer* importer,
        void(*renderSceneContent)(MyWindow&, Importer*));

    void RenderMenuBar(Importer* importer);

    void RenderSceneWindow(MyWindow& window, Importer* importer,
        void(*renderSceneContent)(MyWindow&, Importer*));

    void RenderInspectorWindow();
    void RenderHierarchyWindow();
    void RenderAssetsWindow();
    void RenderConsoleWindow();
    void RenderPerformanceWindow();
    void RenderResourcesWindow();

    bool showHierarchy = true;


private:
    Hierarchy hierarchy;
    Inspector inspector;
    Biblio biblio;
    Console console;
    PerformanceWindow performance;
	ResourceUsageWindow resourceUsage;

    struct WindowStates {
        bool showScene = true;
        bool showGame = false;
        bool showInspector = true;
        bool showHierarchy = true;
        bool showAssets = true;
        bool showConsole = true;
        bool showPerformance = true;
		bool showResources = true;
    } windowStates;

    void ToggleWindow(const char* name, bool& state);
    void CloseWindow(const char* name, bool& state);

};