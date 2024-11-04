#include "EditScene.h"
#include "MyWindow.h" 
#include "Importer.h"
#include "imgui.h"
#include <SDL2/SDL_events.h>


void render(MyWindow& window, Importer* importer); 

EditScene::EditScene() : hierarchy(), inspector(), assets() {}



void EditScene::RenderSceneWindow(MyWindow& window, Importer* importer) {
    ImGui::Begin("Scene");
    render(window, importer); 
    ImGui::End();
}

void EditScene::RenderInspectorWindow() {
    ImGui::Begin("Inspector");
    inspector.DrawInspectorWindow();
    ImGui::End();
}

void EditScene::RenderHierarchyWindow() {
    ImGui::Begin("Hierarchy");
    hierarchy.DrawHierarchyWindow();
    ImGui::End();
}

void EditScene::RenderAssetsWindow() {
    ImGui::Begin("Assets");
    assets.DrawTransformWindow();
    ImGui::End();
}