#include <exception>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>

// Variables de la c�mara (declaradas en main.cpp)
extern GLfloat cameraX;
extern GLfloat cameraY;
extern GLfloat cameraZ;
extern GLfloat cameraAngleX;
extern GLfloat cameraAngleY;
extern GLfloat cameraAngleZ;
extern bool rotatingCamera;
extern bool movingCamera;



using namespace std;

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height)
{
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

void MyWindow::swapBuffers() const {
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

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                if (SDL_GetModState() & KMOD_ALT) {
                    rotatingCamera = true; // Iniciar la rotaci�n de la c�mara
                }
                else if (SDL_GetModState() & KMOD_CTRL) {
                    movingCamera = true; // Iniciar movimiento de la c�mara
                }
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                rotatingCamera = false; // Detener la rotaci�n de la c�mara
                movingCamera = false; // Detener el movimiento de la c�mara
            }
            break;

        case SDL_MOUSEMOTION:
            if (rotatingCamera) {
                cameraAngleX += event.motion.xrel * 0.2f; // Ajustar la sensibilidad
                cameraAngleY += event.motion.yrel * 0.2f;
                cameraAngleZ += event.motion.yrel * 0.2f;
               
            }
            else if (movingCamera) {
                cameraZ += event.motion.zrel * 0.2f; // Cambiar el movimiento de la c�mara en Z
            }
            break;

        case SDL_MOUSEWHEEL:
            if (event.wheel.y > 0) {
                cameraZ -= 1.0f; // Alejar
            }
            else if (event.wheel.y < 0) {
                cameraZ += 1.0f; // Acercar
            }
            break;

        default:
            ImGui_ImplSDL2_ProcessEvent(&event);
            break;
        }
    }
    return true;
}



void MyWindow::close() {
    // Cerrar recursos
}
