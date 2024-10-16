#include <exception>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include "nfd.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string>

extern GLfloat cameraX;
extern GLfloat cameraY;
extern GLfloat cameraZ;
extern GLfloat cameraAngleX;
extern GLfloat cameraAngleY;
extern GLfloat cameraAngleZ;
extern bool rotatingCamera;
extern bool movingCamera;

using namespace std;

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height) {
    open(title, width, height);
}

MyWindow::~MyWindow() {
    close();
}

void MyWindow::open(const char* title, unsigned short width, unsigned short height) {
    if (isOpen()) return;
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    if (!_window) throw exception(SDL_GetError());

    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) throw exception(SDL_GetError());
    if (SDL_GL_MakeCurrent(_window, _ctx) != 0) throw exception(SDL_GetError());
    if (SDL_GL_SetSwapInterval(1) != 0) throw exception(SDL_GetError());

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(_window, _ctx);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void MyWindow::swapBuffers() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            if (ImGui::MenuItem("Adeu")) {
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Assets")) {
            if (ImGui::MenuItem("Abrir textura")) {
                
                openFileDialog("Textura");
            }
            if (ImGui::MenuItem("Abrir FBX")) {
               
                openFileDialog("FBX");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(static_cast<SDL_Window*>(_window));
}

bool MyWindow::processEvents(IEventProcessor* event_processor) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event_processor) event_processor->processEvent(event);

        switch (event.type) {
        case SDL_QUIT:
            return false;

        case SDL_MOUSEMOTION:
            if (rotatingCamera) {
                cameraAngleX += event.motion.xrel * 0.2f; 
                cameraAngleY += event.motion.yrel * 0.2f;
                cameraAngleZ += event.motion.yrel * 0.2f;
            }
            else if (movingCamera) {
                cameraZ += event.motion.yrel * 0.2f; 
            }
            break;

        case SDL_MOUSEWHEEL:
            if (event.wheel.y > 0) {
                cameraZ -= 1.0f; 
            }
            else if (event.wheel.y < 0) {
                cameraZ += 1.0f; 
            }
            break;

        default:
            ImGui_ImplSDL2_ProcessEvent(&event);
            break;
        }
    }
    return true;
}

void MyWindow::openFileDialog(const char* fileType) {
    NFD_Init(); // Inicializamos NFD

    nfdu8char_t* outPath = NULL;
    nfdresult_t result;

    if (strcmp(fileType, "Textura") == 0) {
        // Abrir dialogo para seleccionar texturas (archivos de imagen)
        nfdu8filteritem_t filters[1] = { { "Imágenes", "png,jpg" } };
        nfdopendialogu8args_t args = { 0 };
        args.filterList = filters;
        args.filterCount = 1;
        result = NFD_OpenDialogU8_With(&outPath, &args);
    }
    else if (strcmp(fileType, "FBX") == 0) {
        // Abrir dialogo para archivos FBX
        nfdu8filteritem_t filters[1] = { { "Modelos 3D", "fbx" } };
        nfdopendialogu8args_t args = { 0 };
        args.filterList = filters;
        args.filterCount = 1;
        result = NFD_OpenDialogU8_With(&outPath, &args);
    }

    if (result == NFD_OKAY) {
        puts("Archivo seleccionado:");
        puts(outPath);
        NFD_FreePathU8(outPath); // Liberar la memoria después de usarla
    }
    else if (result == NFD_CANCEL) {
        puts("El usuario canceló el diálogo.");
    }
    else {
        printf("Error al abrir el diálogo: %s\n", NFD_GetError());
    }

    NFD_Quit(); // Finalizamos NFD
}

void MyWindow::close() {
    // Cerrar recursos
}

