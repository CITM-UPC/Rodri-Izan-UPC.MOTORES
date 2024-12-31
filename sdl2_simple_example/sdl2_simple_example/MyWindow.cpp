#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdexcept>
#include "GameObjectManager.h"

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height, Importer* importer)
    : importer(importer) {
    open(title, width, height);
}



void MyWindow::initImGui() {
    if (_imguiInitialized) return;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    ImGui_ImplSDL2_InitForOpenGL(_window, _ctx);
    ImGui_ImplOpenGL3_Init("#version 130");
    _imguiInitialized = true;
}

MyWindow::~MyWindow() {
    if (_imguiInitialized) {
        shutdownImGui();
    }

    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &renderedTexture);
        glDeleteRenderbuffers(1, &depthRenderbuffer);
    }

    close();
}

void MyWindow::shutdownImGui() {
    if (!_imguiInitialized) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    _imguiInitialized = false;
}

void MyWindow::initFramebuffer(unsigned short width, unsigned short height) {
    _viewportWidth = width;
    _viewportHeight = height;

    // Si ya existe un framebuffer, lo eliminamos
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &renderedTexture);
        glDeleteRenderbuffers(1, &depthRenderbuffer);
    }

    // Crear el framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Crear la textura a la que renderizaremos
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Crear un renderbuffer para el buffer de profundidad
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    // Adjuntar la textura al framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Error: El framebuffer no est� completo.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void MyWindow::resizeFramebuffer(int width, int height) {
    if (width != _viewportWidth || height != _viewportHeight) {
        initFramebuffer(width, height);
    }
}


GLuint MyWindow::getRenderedTexture() const {
    return renderedTexture;
}

void MyWindow::updateSceneSize() {
    ImVec2 availableSize = ImGui::GetContentRegionAvail();

    // No actualizar si el tamaño es 0 o no ha cambiado
    if (availableSize.x <= 0 || availableSize.y <= 0) {
        return;
    }

    int newWidth = static_cast<int>(availableSize.x);
    int newHeight = static_cast<int>(availableSize.y);

    if (newWidth != framebufferWidth || newHeight != framebufferHeight) {
        framebufferWidth = newWidth;
        framebufferHeight = newHeight;
        createFrameBuffer(framebufferWidth, framebufferHeight);
    }
}

void MyWindow::bindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    // Actualizar la matriz de proyección
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspectRatio = getAspectRatio();
    gluPerspective(fov, aspectRatio, nearPlane, farPlane);

    // Volver a la matriz de modelview
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MyWindow::createFrameBuffer(int width, int height) {
    // Si ya existe un framebuffer, lo eliminamos
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &renderedTexture);
        glDeleteRenderbuffers(1, &depthRenderbuffer);
    }

    // Crear el framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Crear la textura
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Crear renderbuffer para profundidad
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    // Adjuntar la textura al framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

    // Verificar que el framebuffer esté completo
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Error: El framebuffer no está completo.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void MyWindow::unbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Vuelve a renderizar en el framebuffer por defecto
}

void MyWindow::open(const char* title, unsigned short width, unsigned short height) {
    if (_window) return;

    // Configurar atributos de OpenGL antes de crear la ventana
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Crear la ventana
    _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!_window) throw std::runtime_error(SDL_GetError());

    // Crear el contexto OpenGL
    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) throw std::runtime_error(SDL_GetError());

    if (SDL_GL_MakeCurrent(_window, _ctx) != 0)
        throw std::runtime_error(SDL_GetError());

    if (SDL_GL_SetSwapInterval(1) != 0)
        throw std::runtime_error(SDL_GetError());

   
    initImGui();
}

void MyWindow::close() {
    shutdownImGui();
    if (_ctx) {
        SDL_GL_DeleteContext(_ctx);
        _ctx = nullptr;
    }
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
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

        // Procesar input según el modo actual
        if (isPlayMode) {
            gameCamera.Update();
        }
        else {
            sceneCamera.ProcessKeyboardInput(keystate);
        }

        switch (event.type) {
        case SDL_QUIT:
            close();
            return false;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_F5) {
                SetPlayMode(!isPlayMode);
            }
            else if (isPlayMode && (event.key.keysym.sym == SDLK_ESCAPE ||
                event.key.keysym.sym == SDLK_p)) {
                SetPlayMode(false);  // Salir del modo play con ESC o P
            }
            else if (!isPlayMode && event.key.keysym.sym == SDLK_f) {
                sceneCamera.FocusOnObject();
            }
            break;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (!isPlayMode) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    sceneCamera.SetRotatingCamera(true);
                }
                if (event.button.button == SDL_BUTTON_LEFT && keystate[SDL_SCANCODE_LALT]) {
                    sceneCamera.SetRotatingCamera(true);
                }
                if (event.button.button == SDL_BUTTON_MIDDLE && keystate[SDL_SCANCODE_LALT]) {
                    sceneCamera.SetMovingCameraWithMouse(true);
                }
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (!isPlayMode) {
                sceneCamera.SetRotatingCamera(false);
                sceneCamera.SetMovingCameraWithMouse(false);
            }
            break;

        case SDL_MOUSEMOTION:
            if (!isPlayMode) {
                sceneCamera.ProcessMouseMotion(event.motion.xrel, event.motion.yrel);
            }
            break;

        case SDL_MOUSEWHEEL:
            if (!isPlayMode) {
                sceneCamera.ProcessMouseWheel(event.wheel.y);
            }
            break;

        case SDL_DROPFILE: {
            char* droppedFile = event.drop.file;
            printf("Archivo arrastrado: %s\n", droppedFile);
            HandleDroppedFile(droppedFile);
            SDL_free(droppedFile);
            break;
        }
        }
    }

    return true;
}


void MyWindow::HandleDroppedFile(const char* droppedFile) {
    std::string fileExtension = std::string(droppedFile);
    fileExtension = fileExtension.substr(fileExtension.find_last_of(".") + 1);
    // Convertir a minúsculas para comparación
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

    if (fileExtension == "fbx" || fileExtension =="mdl" || fileExtension == "msh") {
        // Importar el modelo
        if (importer->ImportFBX(droppedFile)) {
            auto& manager = GameObjectManager::GetInstance();
            const std::string modelname = importer->GetModelName(droppedFile);
            const auto* model = importer->GetModel(modelname);
            if (model)
            {
                auto* obj = manager.CreateGameObject<RenderableGameObject>(modelname);

                for (size_t i = 0; i < model->meshes.size(); i++)
                {
                    obj->SetMeshIndex(i);
                }
                obj->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
                obj->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
            }
            sceneCamera.FocusOnObject();
        }
    }
    else if (fileExtension == "png" || fileExtension == "dds" || fileExtension == "jpg" || fileExtension == "jpeg" || fileExtension == "tex") {
        // Importar la textura
        if (importer->ImportTexture(droppedFile)) {
            auto& manager = GameObjectManager::GetInstance();
			      const std::string textureName = importer->GetTextureName(droppedFile);
			      const auto* texture = importer->GetTexture(textureName);
            if (texture) {
                // Obtener el GameObject seleccionado
                GameObject* selectedGameObject = manager.GetSelectedGameObject();
                if (selectedGameObject) {
                    // Comprobar si el GameObject seleccionado es de tipo RenderableGameObject
                    if (RenderableGameObject* renderableObj = dynamic_cast<RenderableGameObject*>(selectedGameObject)) {
                        // Asignar la textura al objeto renderizable seleccionado
						            renderableObj->SetTextureID(texture->textureID);
                    }
                }
            }
        }
    }
}

void MyWindow::HandleDragDropTarget() {
   
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Biblio::GetDragDropType())) {
            // Asegúrate de que el payload es válido
            if (payload->DataSize > 0) {
                // El payload contiene la ruta del archivo
                const char* droppedFile = static_cast<const char*>(payload->Data);
                HandleDroppedFile(droppedFile);
            }
        }
        ImGui::EndDragDropTarget();
    }
}




void MyWindow::swapBuffers() {
    SDL_GL_SwapWindow(_window);
}
