#include "EditScene.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

EditScene::EditScene() {
}

void EditScene::RenderEditorWindows(MyWindow& window, Importer* importer,
    void(*renderSceneContent)(MyWindow&, Importer*)) {

    // 1. Configurar ImGui
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // 2. Renderizar MenuBar primero
    RenderMenuBar();

    // 3. Crear DockSpace después del MenuBar
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    // 4. Renderizar las demás ventanas
    RenderSceneWindow(window, importer, renderSceneContent);
    RenderInspectorWindow();
    RenderHierarchyWindow();
    RenderAssetsWindow();
    RenderPerformanceWindow();

    ImGui::End();
}

void EditScene::RenderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                // Handle New Scene
            }
            if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
                // Handle Open Scene
            }
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                // Handle Save Scene
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                // Handle Exit
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Select All", "Ctrl+A")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Scene", nullptr, true)) {}
            if (ImGui::MenuItem("Game", nullptr, false)) {}
            if (ImGui::MenuItem("Inspector", nullptr, true)) {}
            if (ImGui::MenuItem("Hierarchy", nullptr, true)) {}
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}


void EditScene::RenderSceneWindow(MyWindow& window, Importer* importer,
    void(*renderSceneContent)(MyWindow&, Importer*)) {
    ImGui::Begin("Scene");

    ImVec2 contentRegion = ImGui::GetContentRegionAvail();
    if (contentRegion.x > 0 && contentRegion.y > 0) {
        window.resizeFramebuffer(static_cast<int>(contentRegion.x),
            static_cast<int>(contentRegion.y));

        window.bindFramebuffer();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderSceneContent(window, importer);
        window.unbindFramebuffer();

        ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(window.getRenderedTexture())),
            contentRegion);
    }

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