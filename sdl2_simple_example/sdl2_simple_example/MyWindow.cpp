#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h> // Asegúrate de incluir esto
#include "nfd.h" 
#include <stdio.h>
#include <stdexcept> // Para std::exception

using namespace std;

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height) {
    open(title, width, height);
}

MyWindow::~MyWindow() {
    close();
}

void MyWindow::open(const char* title, unsigned short width, unsigned short height) {
    if (_window) return; // Si ya está abierto, no hacer nada

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    if (!_window) throw std::runtime_error(SDL_GetError()); // Cambiado a std::runtime_error

    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) throw std::runtime_error(SDL_GetError()); // Cambiado a std::runtime_error
    if (SDL_GL_MakeCurrent(_window, _ctx) != 0) throw std::runtime_error(SDL_GetError()); // Cambiado a std::runtime_error
    if (SDL_GL_SetSwapInterval(1) != 0) throw std::runtime_error(SDL_GetError()); // Cambiado a std::runtime_error

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(_window, _ctx);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void MyWindow::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
            close(); // Cerrar la ventana si se recibe un evento de salir
        }
    }
}

void MyWindow::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Asegúrate de que esto funcione correctamente

    // ImGui renderizado
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Crear barra de menú
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

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MyWindow::swapBuffers() {
    SDL_GL_SwapWindow(_window);
}

void MyWindow::close() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(_ctx);
    SDL_DestroyWindow(_window);
    _window = nullptr;
}
