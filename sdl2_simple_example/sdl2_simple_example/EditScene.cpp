#include "EditScene.h"
#include "MyWindow.h" 
#include "Importer.h"
#include "imgui.h"
#include <SDL2/SDL_events.h>


void render(MyWindow& window, Importer* importer); 

EditScene::EditScene() : hierarchy(), inspector(), assets() {}

void EditScene::RenderSceneWindow(MyWindow& window, Importer* importer) {
    ImGui::Begin("Scene");

    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    // Actualizar el framebuffer si es necesario
    window.resizeFramebuffer(static_cast<int>(contentRegion.x),
        static_cast<int>(contentRegion.y));

    // Renderizar la escena en el framebuffer
    window.bindFramebuffer();
    render(window, importer);
    window.unbindFramebuffer();

    // Mostrar la textura renderizada
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(window.getRenderedTexture())),
        contentRegion);

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