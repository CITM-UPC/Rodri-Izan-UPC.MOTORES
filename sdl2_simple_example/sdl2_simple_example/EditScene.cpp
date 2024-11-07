#include "EditScene.h"
#include "MyWindow.h"
#include "Importer.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

EditScene::EditScene() {
    // Implementación del constructor
}

void EditScene::RenderEditorWindows(MyWindow& window, Importer* importer, void(*renderSceneContent)(MyWindow&, Importer*, const std::vector<RenderableGameObject>&), const std::vector<RenderableGameObject>& gameObjects) {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGuiID dockspace_id = main_viewport->ID;
    ImGui::DockSpaceOverViewport(dockspace_id);

    RenderSceneWindow(window, importer, renderSceneContent, gameObjects);
    RenderInspectorWindow();
    RenderHierarchyWindow();
    RenderAssetsWindow();
}

void EditScene::RenderSceneWindow(MyWindow& window, Importer* importer, void(*renderSceneContent)(MyWindow&, Importer*, const std::vector<RenderableGameObject>&), const std::vector<RenderableGameObject>& gameObjects) {
    ImGui::Begin("Scene");
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();
    window.resizeFramebuffer(static_cast<int>(contentRegion.x), static_cast<int>(contentRegion.y));

    window.bindFramebuffer();
    renderSceneContent(window, importer, gameObjects);
    window.unbindFramebuffer();

    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(window.getRenderedTexture())), contentRegion);
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
