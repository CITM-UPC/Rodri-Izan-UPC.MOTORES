#pragma once
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Importer.h"
#include "Biblio.h"
#include "SceneCamera.h"
#include "GameObjectCamera.h"

// Clase abstracta que se encarga de procesar eventos
class IEventProcessor {
public:
    virtual void processEvent(const SDL_Event& event) = 0;
};

// Clase que se encarga de mostrar la ventana principal
class MyWindow {
private:
    Importer* importer;
    SceneCamera sceneCamera;

    GameObjectCamera gameCamera;
    bool isPlayMode = false;

    SDL_Window* _window = nullptr;
    SDL_GLContext _ctx = nullptr;
    unsigned short _width = 800;
    unsigned short _height = 600;

    bool _imguiInitialized = false;

    GLuint framebuffer = 0;
    GLuint renderedTexture = 0;
    GLuint depthRenderbuffer = 0;

    int _viewportWidth = 0;
    int _viewportHeight = 0;

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    void HandleDroppedFile(const char* droppedFile);

public:
    // Getters for camera properties
    glm::vec3 GetCameraPosition() const {
        return isPlayMode ? gameCamera.GetPosition() : sceneCamera.GetPosition();
    }

    glm::vec3 GetTargetPosition() const {
        return isPlayMode ? gameCamera.GetTarget() : sceneCamera.GetTarget();
    }

    void SetPlayMode(bool enabled) {
        isPlayMode = enabled;
        if (isPlayMode) {
            sceneCamera.SetActive(false);
            gameCamera.OnPlay();
        }
        else {
            gameCamera.OnStop();
            sceneCamera.SetActive(true);
        }
    }

    bool IsInPlayMode() const { return isPlayMode; }


    int width() const { return _width; }
    int height() const { return _height; }
    double aspect() const { return static_cast<double>(_width) / _height; }
    SDL_Window* getSDLWindow() const { return _window; }
    SDL_GLContext getSDLContext() const { return _ctx; }

    MyWindow(const char* title, unsigned short width, unsigned short height, Importer* importer);
    MyWindow(MyWindow&&) noexcept = delete;
    MyWindow(const MyWindow&) = delete;
    MyWindow& operator=(const MyWindow&) = delete;
    ~MyWindow();

    void open(const char* title, unsigned short width, unsigned short height);
    void close();
    bool isOpen() const { return _window; }
    void initImGui();
    void shutdownImGui();
    void resizeFramebuffer(int width, int height);
    void initFramebuffer(unsigned short width, unsigned short height);
    GLuint getRenderedTexture() const;
    void unbindFramebuffer();
    void bindFramebuffer();
    bool processEvents(IEventProcessor* event_processor = nullptr);
    void swapBuffers();

    void updateSceneSize();
    void createFrameBuffer(int width, int height);
    float getAspectRatio() const { return static_cast<float>(framebufferWidth) / framebufferHeight; }
    int getFramebufferWidth() const { return framebufferWidth; }
    int getFramebufferHeight() const { return framebufferHeight; }

    void HandleDragDropTarget();

};