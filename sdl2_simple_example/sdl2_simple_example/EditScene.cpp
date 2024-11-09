#include "EditScene.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

EditScene::EditScene() {
    // Implementación del constructor
}

void EditScene::RenderEditorWindows(MyWindow& window, Importer* importer,
    void(*renderSceneContent)(MyWindow&, Importer*)) {

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGuiID dockspace_id = main_viewport->ID;
    ImGui::DockSpaceOverViewport(dockspace_id);

    RenderSceneWindow(window, importer, renderSceneContent);
    RenderInspectorWindow();
    RenderHierarchyWindow();
    RenderAssetsWindow();
    RenderPerformanceWindow();
}

void EditScene::RenderSceneWindow(MyWindow& window, Importer* importer,
    void(*renderSceneContent)(MyWindow&, Importer*)) {

    ImGui::Begin("Scene");

    ImVec2 contentRegion = ImGui::GetContentRegionAvail();
    window.resizeFramebuffer(static_cast<int>(contentRegion.x), static_cast<int>(contentRegion.y));

    window.bindFramebuffer();
    renderSceneContent(window, importer);
    window.unbindFramebuffer();

    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(window.getRenderedTexture())),
        contentRegion);

    ImGui::End();
}

void EditScene::RenderInspectorWindow() {
    ImGui::Begin("Inspector");
    // Obtener el objeto seleccionado del GameObjectManager si es necesario
    auto& manager = GameObjectManager::GetInstance();
    // Aquí podrías obtener el objeto seleccionado y pasarlo al inspector
    inspector.DrawInspectorWindow();
    ImGui::End();
}

void EditScene::RenderHierarchyWindow() {
    ImGui::Begin("Hierarchy");
    auto& manager = GameObjectManager::GetInstance();
    auto allObjects = manager.GetAllGameObjects();
    // Aquí podrías pasar la lista de objetos a la jerarquía
    hierarchy.DrawHierarchyWindow();
    ImGui::End();
}

void EditScene::RenderAssetsWindow() {
    ImGui::Begin("Assets");
    assets.DrawTransformWindow();
    ImGui::End();
}

void EditScene::RenderPerformanceWindow() {
    ImGui::Begin("Performance Monitor");
	performance.DrawPerformanceWindow();
	ImGui::End();
}