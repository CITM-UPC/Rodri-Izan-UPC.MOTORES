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

// Archivso para crear la casa al inicio
const char* filefbx = "./Assets/BakerHouse.fbx"; 
const char* filetex = "./Assets/Baker_house.png";

EditScene editor;

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
    // Comenzar la ventana Scene
    ImGui::Begin("Scene");

    // Obtener el tamaño disponible para la ventana Scene
    ImVec2 availableSize = ImGui::GetContentRegionAvail();

    // Renderizar la textura del framebuffer
    ImGui::Image((void*)(intptr_t)window.getRenderedTexture(),
        availableSize,
        ImVec2(0, 1),
        ImVec2(1, 0));

    // Implementar drag & drop en la ventana Scene
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
            const char* path = static_cast<const char*>(payload->Data);
            std::string filePath(path);
            std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

            // Manejar diferentes tipos de archivos
            if (extension == "fbx") {
                if (importer->ImportFBX(filePath.c_str())) {
                    auto& manager = GameObjectManager::GetInstance();
                    auto* obj = manager.CreateGameObject<RenderableGameObject>("DraggedModel");
                    const auto& meshes = importer->GetMeshes();
                    for (size_t i = 0; i < meshes.size(); i++) {
                        obj->SetMeshIndex(i);
                    }
                    obj->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
                    obj->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));

                    // Calcular la posición en el mundo basada en la posición del ratón
                    ImVec2 mousePos = ImGui::GetMousePos();
                    ImVec2 windowPos = ImGui::GetWindowPos();
                    ImVec2 relativePos = ImVec2(mousePos.x - windowPos.x, mousePos.y - windowPos.y);

                    // Convertir la posición 2D del ratón a una posición 3D en el mundo
                    // Esta es una implementación simple, podrías mejorarla según tus necesidades
                    float worldX = (relativePos.x / availableSize.x - 0.5f) * 20.0f;
                    float worldZ = (relativePos.y / availableSize.y - 0.5f) * 20.0f;
                    obj->SetPosition(glm::vec3(worldX, 0.0f, worldZ));
                }
            }
            else if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "dds") {
                if (importer->ImportTexture(filePath.c_str())) {
                    auto& manager = GameObjectManager::GetInstance();
                    if (GameObject* selectedObj = manager.GetSelectedGameObject()) {
                        if (RenderableGameObject* renderableObj = dynamic_cast<RenderableGameObject*>(selectedObj)) {
                            renderableObj->SetTextureID(importer->GetTextureID());
                        }
                    }
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::End();

    // Renderizar la escena 3D
    window.bindFramebuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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
        glm::mat4 transform = gameObject->GetTransformMatrix();
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

    return 0;
}
