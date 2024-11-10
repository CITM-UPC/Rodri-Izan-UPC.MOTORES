#pragma once
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Importer.h"

// Clase abstracta que se encarga de procesar eventos
class IEventProcessor {
public:
    virtual void processEvent(const SDL_Event& event) = 0;
};

// Clase que se encarga de mostrar la ventana principal
class MyWindow {
private:
    Importer* importer;

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

    // Camera properties
    GLfloat cameraX = 0.0f;
    GLfloat cameraY = 5.0f;
    GLfloat cameraZ = 10.0f;
    GLfloat cameraSpeed = 0.1f;

    GLfloat cameraAngleX = 0.0f;
    GLfloat cameraAngleY = 0.0f;
    GLfloat cameraAngleZ = 0.0f;
    bool rotatingCamera = false;
    bool movingCamera = false;
    bool movingCameraWithMouse = false;

    GLfloat targetX = 0.0f;
    GLfloat targetY = 0.0f;
    GLfloat targetZ = 0.0f;
    GLfloat orbitRadius = 10.0f;
    GLfloat orbitAngleHorizontal = 0.0f;
    GLfloat orbitAngleVertical = 30.0f;

    // Private camera methods
    void MoveCamera(const Uint8* keystate);
    void MoveCameraWithMouse(int xrel, int yrel);

    void RotateCamera(int xrel, int yrel);
    void FocusOnObject();
    void MoveCameraWithMouseWheel(int wheelDirection, const Uint8* keystate);
    void HandleDroppedFile(const char* droppedFile);

public:
    // Getters for camera properties
    glm::vec3 GetCameraPosition() const { return glm::vec3(cameraX, cameraY, cameraZ); }
    glm::vec3 GetTargetPosition() const { return glm::vec3(targetX, targetY, targetZ); }


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

};