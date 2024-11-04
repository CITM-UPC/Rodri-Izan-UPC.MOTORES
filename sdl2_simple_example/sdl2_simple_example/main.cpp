#include <GL/glew.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "Importer.h"
#include <imgui_impl_sdl2.h>
#include "EditScene.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GLuint textureID;

using namespace std;
using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(1024, 1024);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;  

const char* filefbx = "../Assets/BakerHouse.fbx";
const char* filefbx1 = "../Assets/masterchief.fbx";
const char* filetex = "../Assets/Baker_house.png";

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

void render(MyWindow& window, Importer* importer) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Get camera position from window
    auto camPos = window.GetCameraPosition();
    auto targetPos = window.GetTargetPosition();

    gluLookAt(camPos.x, camPos.y, camPos.z,
        targetPos.x, targetPos.y, targetPos.z,
        0.0f, 1.0f, 0.0f);

    drawGrid(10.0f, 20);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(0.1f, 0.1f, 0.1f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, importer->GetTextureID());

    for (const auto& mesh : importer->GetMeshes()) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, mesh.vertices.data());

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, mesh.texCoords.data());

        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, mesh.indices.data());

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glFlush();
}

int main(int argc, char** argv) {
    Importer importer;  
    // Inicializa la ventana usando SDL
    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y, &importer);  // Pasar importer a MyWindow

    // Inicializa OpenGL
    init_openGL();

    // Crear el contexto de Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Configurar estilo de Dear ImGui
    ImGui::StyleColorsDark();

    // Inicializar el backend de SDL y OpenGL para Dear ImGui
    ImGui_ImplSDL2_InitForOpenGL(window.getSDLWindow(), window.contextPtr());
    ImGui_ImplOpenGL3_Init("#version 130"); // Asegúrate de que la versión de OpenGL sea la adecuada

    // Inicializar el Importer
    importer.Init();
    importer.ImportFBX(filefbx);
    //importer.ImportFBX(filefbx1);
    importer.ImportTexture(filetex);

    // Bucle principal
    while (window.processEvents() && window.isOpen()) {
        // Comienza un nuevo frame de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();  // Pasa la ventana de SDL
        ImGui::NewFrame();

        // Renderiza la interfaz de usuario usando Dear ImGui
        RenderEditor();

        // Renderiza la escena 3D
        render(window, &importer);

        // Renderiza los datos de Dear ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Intercambia buffers
        window.swapBuffers();
    }

    // Limpieza de Dear ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

