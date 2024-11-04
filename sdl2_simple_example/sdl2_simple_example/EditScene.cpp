#include "EditScene.h"
#include "imgui.h"

EditScene::EditScene() : hierarchy(), inspector(), assets() {}

void EditScene::RenderEditor() {
    hierarchy.DrawHierarchyWindow();
    inspector.DrawInspectorWindow();
    assets.DrawTransformWindow();
}