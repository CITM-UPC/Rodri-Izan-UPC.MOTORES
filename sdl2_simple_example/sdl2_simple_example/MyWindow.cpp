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

void MyWindow::MoveCamera(const Uint8* keystate) {
    float adjustedSpeed = (keystate[SDL_SCANCODE_LSHIFT] ? cameraSpeed * 2.0f : cameraSpeed);
    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    glm::vec3 forward(
        cos(verticalRad) * sin(horizontalRad),
        sin(verticalRad),
        cos(verticalRad) * cos(horizontalRad)
    );

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Movimiento adelante y atr�s
    if (keystate[SDL_SCANCODE_W]) {
        cameraX -= forward.x * adjustedSpeed;
        cameraY -= forward.y * adjustedSpeed;
        cameraZ -= forward.z * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_S]) {
        cameraX += forward.x * adjustedSpeed;
        cameraY += forward.y * adjustedSpeed;
        cameraZ += forward.z * adjustedSpeed;
    }

    // Movimiento lateral
    if (keystate[SDL_SCANCODE_D]) {
        cameraX -= right.x * adjustedSpeed;
        cameraY -= right.y * adjustedSpeed;
        cameraZ -= right.z * adjustedSpeed;
    }
    if (keystate[SDL_SCANCODE_A]) {
        cameraX += right.x * adjustedSpeed;
        cameraY += right.y * adjustedSpeed;
        cameraZ += right.z * adjustedSpeed;
    }
}

void MyWindow::RotateCamera(int xrel, int yrel) {
    const float sensitivity = 0.3f;

    orbitAngleHorizontal += xrel * sensitivity;
    orbitAngleVertical += yrel * sensitivity;

    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    // Actualizar la posici�n de la c�mara basada en los �ngulos
    cameraX = targetX + orbitRadius * cos(verticalRad) * sin(horizontalRad);
    cameraY = targetY + orbitRadius * sin(verticalRad);
    cameraZ = targetZ + orbitRadius * cos(verticalRad) * cos(horizontalRad);
}
void MyWindow::FocusOnObject() {
    // Cambia targetX, Y, Z seg�n el objeto que deseas centrar
    targetX = 0.0f;
    targetY = 0.0f;
    targetZ = 0.0f;
    orbitAngleHorizontal = 0.0f;
    orbitAngleVertical = -30.0f;
    orbitRadius = 10.0f;  // Ajusta el radio seg�n el tama�o del objeto
    RotateCamera(0, 0);
}
void MyWindow::MoveCameraWithMouse(int xrel, int yrel) {
    float sensitivity = 0.05f; 
    glm::vec3 up(0.0f, 1.0f, 0.0f);  // Eje vertical para mover en el eje Y
    glm::vec3 right(-1.0f, 0.0f, 0.0f);  // Eje vertical para mover en el eje X

    // C�lculo del desplazamiento en el eje horizontal y vertical
    glm::vec3 horizontalMovement = right * (-xrel * sensitivity); 
    glm::vec3 verticalMovement = up * (yrel * sensitivity);        

    // Actualizar la posici�n de la c�mara y el objetivo
    cameraX += horizontalMovement.x + verticalMovement.x;
    cameraY += horizontalMovement.y + verticalMovement.y;
    cameraZ += horizontalMovement.z + verticalMovement.z;

    targetX += horizontalMovement.x + verticalMovement.x;
    targetY += horizontalMovement.y + verticalMovement.y;
    targetZ += horizontalMovement.z + verticalMovement.z;
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
            close();
            return false;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                _width = event.window.data1;
                _height = event.window.data2;
                resizeFramebuffer(_width, _height);
            }
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_f) {
                FocusOnObject();
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_RIGHT) {
                rotatingCamera = true;
            }
            if (event.button.button == SDL_BUTTON_LEFT && keystate[SDL_SCANCODE_LALT]) {
                rotatingCamera = true;
            }
            if (event.button.button == SDL_BUTTON_MIDDLE && keystate[SDL_SCANCODE_LALT]) {
                movingCameraWithMouse = true;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            rotatingCamera = false;
            movingCameraWithMouse = false;
            break;

        case SDL_MOUSEMOTION:
            if (rotatingCamera) {
                RotateCamera(event.motion.xrel, event.motion.yrel);
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

    if (fileExtension == "fbx") {
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
            FocusOnObject();
        }
    }
    else if (fileExtension == "png" || fileExtension == "dds" || fileExtension == "jpg" || fileExtension == "jpeg") {
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



void MyWindow::swapBuffers() {
    SDL_GL_SwapWindow(_window);
}
