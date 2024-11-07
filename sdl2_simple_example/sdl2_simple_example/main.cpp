#include <GL/glew.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "Importer.h"
#include "Biblio.h"
#include "Hierarchy.h"
#include <imgui_impl_sdl2.h>
#include "EditScene.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include "GameObjectManager.h"

using namespace std;
using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(720, 720);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

const char* filefbx = "../Assets/BakerHouse.fbx";
const char* filefbx1 = "../Assets/masterchief.fbx";
const char* filetex = "../Assets/Baker_house.png";

EditScene editor;

static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);
}

void drawGrid(float gridSize, int gridDivisions) {
    float halfSize = gridSize * 0.5f;
    float step = gridSize / gridDivisions;

    glBegin(GL_LINES);
    glColor3f(0.6f, 0.6f, 0.6f); // Color gris para el grid

    for (int i = 0; i <= gridDivisions; i++) {
        float pos = -halfSize + i * step;

        // Líneas paralelas al eje Z
        glVertex3f(pos, 0.0f, -halfSize);
        glVertex3f(pos, 0.0f, halfSize);

        // Líneas paralelas al eje X
        glVertex3f(-halfSize, 0.0f, pos);
        glVertex3f(halfSize, 0.0f, pos);
    }

    glEnd();
}


void renderSceneContent(MyWindow& window, Importer* importer) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configuración de proyección y vista
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    auto camPos = window.GetCameraPosition();
    auto targetPos = window.GetTargetPosition();
    gluLookAt(camPos.x, camPos.y, camPos.z, targetPos.x, targetPos.y, targetPos.z, 0.0f, 1.0f, 0.0f);

    drawGrid(10.0f, 20);

    // Obtener los objetos renderizables del manager
    auto& manager = GameObjectManager::GetInstance();
    auto renderableObjects = manager.GetGameObjectsOfType<RenderableGameObject>();

    // Renderizar cada GameObject
    for (const auto* gameObject : renderableObjects) {
        if (!gameObject->IsActive()) continue;

        glPushMatrix();

        // Aplicar transformación del GameObject
        glm::mat4 transform = gameObject->GetTransformMatrix();
        glMultMatrixf(glm::value_ptr(transform));

        // Activar textura si existe
        if (gameObject->GetTextureID() != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, gameObject->GetTextureID());
        }

        // Renderizar la malla asociada
        const auto& meshes = importer->GetMeshes();
        if (gameObject->GetMeshIndex() >= 0 && gameObject->GetMeshIndex() < meshes.size()) {
            const auto& mesh = meshes[gameObject->GetMeshIndex()];

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, mesh.vertices.data());

            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, mesh.texCoords.data());

            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, mesh.indices.data());

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        if (gameObject->GetTextureID() != 0) {
            glDisable(GL_TEXTURE_2D);
        }

        glPopMatrix();
    }
    glFlush();
}

int main(int argc, char** argv) {
    Importer importer;
    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y, &importer);

    init_openGL();
    importer.Init();
    importer.ImportFBX(filefbx);
    importer.ImportTexture(filetex);

    // Crear GameObjects de ejemplo
    auto& manager = GameObjectManager::GetInstance();

    // Crear objetos renderizables
    const auto& meshes = importer.GetMeshes();
    for (size_t i = 0; i < meshes.size(); i++) {
        auto* obj = manager.CreateGameObject<RenderableGameObject>("House_Part_" + std::to_string(i));
        obj->SetMeshIndex(i);
        obj->SetTextureID(importer.GetTextureID());
        obj->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
        obj->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    }
    // Bucle principal
    while (window.processEvents() && window.isOpen()) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();



        editor.RenderEditorWindows(window, &importer, renderSceneContent);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(window.getSDLWindow(), window.getSDLContext());
        }

        window.swapBuffers();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
