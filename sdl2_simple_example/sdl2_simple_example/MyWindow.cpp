#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdexcept>


MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height) {
    open(title, width, height);
}

MyWindow::~MyWindow() {
    if (_imguiInitialized) {
        shutdownImGui();
    }
    close();
}

void MyWindow::open(const char* title, unsigned short width, unsigned short height) {
    if (_window) return;

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!_window) throw std::runtime_error(SDL_GetError());

    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) throw std::runtime_error(SDL_GetError());

    if (SDL_GL_MakeCurrent(_window, _ctx) != 0)
        throw std::runtime_error(SDL_GetError());

    if (SDL_GL_SetSwapInterval(1) != 0)
        throw std::runtime_error(SDL_GetError());
}

void MyWindow::initImGui() {
    if (_imguiInitialized) return;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForOpenGL(_window, _ctx);
    ImGui_ImplOpenGL3_Init("#version 130");
    _imguiInitialized = true;
}

void MyWindow::shutdownImGui() {
    if (!_imguiInitialized) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    _imguiInitialized = false;
}

void MyWindow::close() {
    if (_ctx) {
        SDL_GL_DeleteContext(_ctx);
        _ctx = nullptr;
    }
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
}

void MyWindow::MoveCamera(const Uint8* keystate) {
    float adjustedSpeed = cameraSpeed;
    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    glm::vec3 forward(
        cos(verticalRad) * sin(horizontalRad),
        sin(verticalRad),
        cos(verticalRad) * cos(horizontalRad)
    );

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    if (keystate[SDL_SCANCODE_S]) {
        cameraX += forward.x * adjustedSpeed;
        cameraY += forward.y * adjustedSpeed;
        cameraZ += forward.z * adjustedSpeed;

        targetX += forward.x * adjustedSpeed;
        targetY += forward.y * adjustedSpeed;
        targetZ += forward.z * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_W]) {
        cameraX -= forward.x * adjustedSpeed;
        cameraY -= forward.y * adjustedSpeed;
        cameraZ -= forward.z * adjustedSpeed;

        targetX -= forward.x * adjustedSpeed;
        targetY -= forward.y * adjustedSpeed;
        targetZ -= forward.z * adjustedSpeed;
    }


    if (keystate[SDL_SCANCODE_D]) {
        cameraX -= right.x * adjustedSpeed;
        cameraY -= right.y * adjustedSpeed;
        cameraZ -= right.z * adjustedSpeed;

        targetX -= right.x * adjustedSpeed;
        targetY -= right.y * adjustedSpeed;
        targetZ -= right.z * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_A]) {
        cameraX += right.x * adjustedSpeed;
        cameraY += right.y * adjustedSpeed;
        cameraZ += right.z * adjustedSpeed;

        targetX += right.x * adjustedSpeed;
        targetY += right.y * adjustedSpeed;
        targetZ += right.z * adjustedSpeed;
    }

    if (keystate[SDL_SCANCODE_Q]) {
        cameraY += adjustedSpeed;
        targetY += adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_E]) {
        cameraY -= adjustedSpeed;
        targetY -= adjustedSpeed;
    }
}

void MyWindow::MoveCameraWithMouse(int xrel, int yrel) {
    float sensitivity = cameraSpeed * 0.5f;
    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    glm::vec3 forward(
        cos(verticalRad) * sin(horizontalRad),
        sin(verticalRad),
        cos(verticalRad) * cos(horizontalRad)
    );

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    float moveX = -xrel * sensitivity;
    float moveZ = -yrel * sensitivity;

    cameraX += right.x * moveX + forward.x * moveZ;
    cameraY += right.y * moveX + forward.y * moveZ;
    cameraZ += right.z * moveX + forward.z * moveZ;

    targetX += right.x * moveX + forward.x * moveZ;
    targetY += right.y * moveX + forward.y * moveZ;
    targetZ += right.z * moveX + forward.z * moveZ;
}

void MyWindow::RotateCamera(int xrel, int yrel) {
    const float sensitivity = 0.3f;
    orbitAngleHorizontal += xrel * sensitivity;
    orbitAngleVertical = glm::clamp(orbitAngleVertical + yrel * sensitivity, -89.0f, 89.0f);

    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    cameraX = targetX + orbitRadius * cos(verticalRad) * sin(horizontalRad);
    cameraY = targetY + orbitRadius * sin(verticalRad);
    cameraZ = targetZ + orbitRadius * cos(verticalRad) * cos(horizontalRad);
}

void MyWindow::FocusOnObject() {
    targetX = 0.0f;
    targetY = 0.0f;
    targetZ = 0.0f;
    orbitAngleHorizontal = 0.0f;
    orbitAngleVertical = 30.0f;
    RotateCamera(0, 0);
}

bool MyWindow::processEvents(IEventProcessor* event_processor) {
    SDL_Event event;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        if (_imguiInitialized) {
            ImGui_ImplSDL2_ProcessEvent(&event);
        }

        if (event_processor) {
            event_processor->processEvent(event);
        }

        MoveCamera(keystate);

        switch (event.type) {
        case SDL_QUIT:
            return false;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_f) {
                FocusOnObject();
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT && keystate[SDL_SCANCODE_LALT]) {
                rotatingCamera = true;
            }
            if (event.button.button == SDL_BUTTON_MIDDLE && keystate[SDL_SCANCODE_LALT]) {
                movingCameraWithMouse = true;
            }
            if (event.button.button == SDL_BUTTON_LEFT && keystate[SDL_SCANCODE_LCTRL]) {
                movingCamera = true;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                rotatingCamera = false;
                movingCameraWithMouse = false;
            }
            break;

        case SDL_MOUSEMOTION:
            if (rotatingCamera) {
                RotateCamera(event.motion.xrel, event.motion.yrel);
            }
            if (movingCamera) {
                MoveCamera(keystate);
            }
            if (movingCameraWithMouse) {
                MoveCameraWithMouse(event.motion.xrel, event.motion.yrel);
            }
            break;

        case SDL_MOUSEWHEEL:
            if (event.wheel.y > 0) {
                orbitRadius = glm::max(orbitRadius - 1.0f, 2.0f);
            }
            else if (event.wheel.y < 0) {
                orbitRadius = glm::min(orbitRadius + 1.0f, 30.0f);
            }
            RotateCamera(0, 0);
            break;

        case SDL_DROPFILE: {

            char* droppedFile = event.drop.file;
            printf("Archivo arrastrado: %s\n", droppedFile);

  /*        importer->ImportFBX(droppedFile);

            delete importer;*/

            break;
        }
        }
    }
    return true;
}

void MyWindow::swapBuffers() {
    SDL_GL_SwapWindow(_window);
}