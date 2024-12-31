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

EditScene editor;

//ruta de un file fbx
const char* filefbx = "./Assets/Street environment_V01.fbx";

// Inicialización de OpenGL
static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);
}

// Creación de un grid
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
    auto camPos = window.GetCameraPosition();
    auto targetPos = window.GetTargetPosition();
    gluLookAt(camPos.x, camPos.y, camPos.z, targetPos.x, targetPos.y, targetPos.z, 0.0f, 1.0f, 0.0f);

    drawGrid(10.0f, 20);

    // Renderizar GameObjects
    auto& manager = GameObjectManager::GetInstance();
    auto renderableObjects = manager.GetGameObjectsOfType<RenderableGameObject>();

    

    for (const auto* gameObject : renderableObjects) {
        if (!gameObject->IsActive()) continue;
        const auto* model = importer->GetModel(gameObject->GetName());
        if (!model) continue;

        // Aplicar transformación de la cámara
        glPushMatrix();
        glm::mat4 transform = gameObject->GetLocalTransformMatrix();
        glMultMatrixf(glm::value_ptr(transform));

        if (gameObject->GetTextureID() != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, gameObject->GetTextureID());
        }

        // Renderizar todas las mallas asociadas
        for (int meshIndex : gameObject->GetMeshIndices()) {
            if (meshIndex >= 0 && meshIndex < model->meshes.size()) {
                const auto& mesh = model->meshes[meshIndex];

                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(3, GL_FLOAT, 0, mesh.vertices.data());
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(2, GL_FLOAT, 0, mesh.texCoords.data());
                glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, mesh.indices.data());
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        }

        if (gameObject->GetTextureID() != 0) {
            glDisable(GL_TEXTURE_2D);
        }

        glPopMatrix();
    }

    window.unbindFramebuffer();
    glFlush();
}
int main(int argc, char** argv) {
    Importer importer;
    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y, &importer);

    init_openGL();

    // Inicialización de Importer
    importer.Init();

    if (importer.ImportFBX(filefbx)) {
        auto& manager = GameObjectManager::GetInstance();
        const std::string modelname = importer.GetModelName(filefbx);
        const auto* model = importer.GetModel(modelname);

        if (model) {
            auto* obj = manager.CreateGameObject<RenderableGameObject>(modelname);

            // Add all meshes from the model
            for (size_t i = 0; i < model->meshes.size(); i++) {
                obj->SetMeshIndex(static_cast<int>(i));
            }

            // Set default transform
            obj->SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
            obj->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
        }
    }

    // Bucle principal
    while (window.processEvents() && window.isOpen()) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();



        // Llamar a la función de renderizado de la escena, que esta llama al renderizado de los GameObjects
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

    return 0;//
}
