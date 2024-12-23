#include "EditScene.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "stdlib.h"
#include "PrimitiveFactory.h"

EditScene::EditScene()
    : biblio("./Library") { // Inicializamos con una ruta
}

void EditScene::RenderEditorWindows(MyWindow& window, Importer* importer,
    void(*renderSceneContent)(MyWindow&, Importer*)) {

    // 1. Configurar ImGui
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // 2. Renderizar MenuBar primero
    RenderMenuBar();

    // 3. Crear DockSpace despu�s del MenuBar
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

    // 4. Renderizar las dem�s ventanas
    RenderSceneWindow(window, importer, renderSceneContent);
    RenderInspectorWindow();
    RenderHierarchyWindow();
    RenderAssetsWindow();
    RenderPerformanceWindow();
    RenderConsoleWindow();
	RenderResourcesWindow();

    ImGui::End();
}

void EditScene::ToggleWindow(const char* name, bool& state) {
    state = !state;
    if (state) {
        // Si la ventana se est� abriendo, configurarla para que aparezca en el dockspace
        ImGui::SetNextWindowDockID(ImGui::GetID("MyDockSpace"), ImGuiCond_FirstUseEver);
    }
}

void EditScene::CloseWindow(const char* name, bool& state) {
    state = false;
}


void EditScene::RenderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
            }
            if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
            }
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                exit(0);
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
            if (ImGui::MenuItem("Scene", nullptr, windowStates.showScene)) {
                ToggleWindow("Scene", windowStates.showScene);
            }
            if (ImGui::MenuItem("Game", nullptr, windowStates.showGame)) {
                ToggleWindow("Game", windowStates.showGame);
            }
            if (ImGui::MenuItem("Inspector", nullptr, windowStates.showInspector)) {
                ToggleWindow("Inspector", windowStates.showInspector);
            }
            if (ImGui::MenuItem("Hierarchy", nullptr, windowStates.showHierarchy)) {
                ToggleWindow("Hierarchy", windowStates.showHierarchy);
            }
            if (ImGui::MenuItem("Assets", nullptr, windowStates.showAssets)) {
                ToggleWindow("Assets", windowStates.showAssets);
            }
            if (ImGui::MenuItem("Console", nullptr, windowStates.showConsole)) {
                ToggleWindow("Console", windowStates.showConsole);
            }
            if (ImGui::MenuItem("Performance", nullptr, windowStates.showPerformance)) {
                ToggleWindow("Performance", windowStates.showPerformance);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GameObject")) {
            if (ImGui::BeginMenu("3D Object")) {
                if (ImGui::MenuItem("Cube")) {
                    PrimitiveFactory::CreatePrimitive(PrimitiveFactory::PrimitiveType::Cube);
                }
                if (ImGui::MenuItem("Sphere")) {
                    PrimitiveFactory::CreatePrimitive(PrimitiveFactory::PrimitiveType::Sphere);
                }
                if (ImGui::MenuItem("Cylinder")) {
                    PrimitiveFactory::CreatePrimitive(PrimitiveFactory::PrimitiveType::Cylinder);
                }
                if (ImGui::MenuItem("Plane")) {
                    PrimitiveFactory::CreatePrimitive(PrimitiveFactory::PrimitiveType::Plane);
                }
                if (ImGui::MenuItem("Cone")) {
                    PrimitiveFactory::CreatePrimitive(PrimitiveFactory::PrimitiveType::Cone);
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Camera")) {
                PrimitiveFactory::CreateCamera();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void EditScene::RenderSceneWindow(MyWindow& window, Importer* importer,
    void(*renderSceneContent)(MyWindow&, Importer*)) {
    if (!windowStates.showScene) return;

    bool isOpen = true;
    ImGui::Begin("Scene", &isOpen, ImGuiWindowFlags_NoScrollbar);

    if (!isOpen) {
        CloseWindow("Scene", windowStates.showScene);
        ImGui::End();
        return;
    }

    // Actualizar el tama�o de la escena
    window.updateSceneSize();

    ImVec2 contentSize = ImGui::GetContentRegionAvail();

    // Renderizar la escena
    window.bindFramebuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderSceneContent(window, importer);
    window.unbindFramebuffer();

    // Renderizar la textura del framebuffer
    ImGui::Image(
        reinterpret_cast<void*>(static_cast<intptr_t>(window.getRenderedTexture())),
        contentSize,
        ImVec2(0, 0),
        ImVec2(1, 1),
        ImVec4(1, 1, 1, 1),
        ImVec4(0, 0, 0, 0)
    );

    window.HandleDragDropTarget();
    ImGui::End();
}

void EditScene::RenderInspectorWindow() {
    if (!windowStates.showInspector) return;

    bool isOpen = true;
    ImGui::Begin("Inspector", &isOpen);

    if (!isOpen) {
        CloseWindow("Inspector", windowStates.showInspector);
        ImGui::End();
        return;
    }

    inspector.DrawInspectorWindow();
    ImGui::End();
}

void EditScene::RenderHierarchyWindow() {
    if (!windowStates.showHierarchy) return;

    bool isOpen = true;
    ImGui::Begin("Hierarchy", &isOpen);

    if (!isOpen) {
        CloseWindow("Hierarchy", windowStates.showHierarchy);
        ImGui::End();
        return;
    }

    hierarchy.DrawHierarchyWindow();
    ImGui::End();
}

void EditScene::RenderAssetsWindow() {
    if (!windowStates.showAssets) return;

    bool isOpen = true;
    ImGui::Begin("Assets", &isOpen);

    if (!isOpen) {
        CloseWindow("Assets", windowStates.showAssets);
        ImGui::End();
        return;
    }

    biblio.DrawAssetsWindow();
    ImGui::End();
}

void EditScene::RenderConsoleWindow() {
    if (!windowStates.showConsole) return;

    bool isOpen = true;
  
    console.Draw("Console");

    if (!isOpen) {
        CloseWindow("Console", windowStates.showConsole);
        return;
    }

    console.Flush();
}

void EditScene::RenderPerformanceWindow() {
    if (!windowStates.showPerformance) return;

    bool isOpen = true;
    ImGui::Begin("Performance Monitor", &isOpen);

    if (!isOpen) {
        CloseWindow("Performance", windowStates.showPerformance);
        ImGui::End();
        return;
    }

    performance.DrawPerformanceWindow();
    ImGui::End();
}
void EditScene::RenderResourcesWindow() {
    if (!windowStates.showResources) return;

    bool isOpen = true;
    ImGui::Begin("Resources Monitor", &isOpen);

    if (!isOpen) {
        CloseWindow("Resources", windowStates.showResources);
        ImGui::End();
        return;
    }

	resourceUsage.Display();
    ImGui::End();
}