#include "EditScene.h"
#include "MyWindow.h"
#include "Importer.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

EditScene::EditScene() {
    // Implementación del constructor
}

void EditScene::RenderEditorWindows(MyWindow& window, Importer* importer, void(*renderSceneContent)(MyWindow&, Importer*)) {
    // Habilitar el docking global
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Obtener el ID del viewport principal
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGuiID dockspace_id = main_viewport->ID;

    // Crear el espacio de anclaje
    ImGui::DockSpaceOverViewport(dockspace_id);

    // Renderizar las ventanas del editor
    RenderSceneWindow(window, importer, renderSceneContent);
    RenderInspectorWindow();
    RenderHierarchyWindow();
    RenderAssetsWindow();
}

void EditScene::RenderSceneWindow(MyWindow& window, Importer* importer, void(*renderSceneContent)(MyWindow&, Importer*)) {
    ImGui::Begin("Scene");
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();
    window.resizeFramebuffer(static_cast<int>(contentRegion.x), static_cast<int>(contentRegion.y));

    // Enlazar el framebuffer antes de renderizar la escena
    window.bindFramebuffer();

    // Llamar a la función de renderizado
    renderSceneContent(window, importer);

    // Desenlazar el framebuffer para volver a dibujar en la pantalla principal
    window.unbindFramebuffer();

    // Mostrar la textura renderizada en la ventana
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
