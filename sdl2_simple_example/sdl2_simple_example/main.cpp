#include <GL/glew.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include <imgui_impl_sdl2.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint textureID;

using namespace std;
using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(1024, 1024);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;


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


//const char* file = "C:/Users/rodrigoam/Documents/GitHub/Rodri-Izan-UPC.MOTORES/Assets/masterchief.fbx";
const char* filefbx = "C:/Users/izansl/Documents/GitHub/Rodri-Izan-UPC.MOTORES/Assets/BakerHouse.fbx";
const char* filetex = "C:/Users/izansl/Documents/GitHub/Rodri-Izan-UPC.MOTORES/Assets/Baker_house.png";

struct Mesh {
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> texCoords;
    std::vector<GLuint> indices;
};

std::vector<Mesh> meshes;

void loadFBX(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error al cargar el modelo: " << importer.GetErrorString() << std::endl;
        return;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh* aimesh = scene->mMeshes[i];
        std::cout << "Malla " << i << " cargada con " << aimesh->mNumVertices << " vértices." << std::endl;

        Mesh mesh;

        for (unsigned int v = 0; v < aimesh->mNumVertices; v++) {
            mesh.vertices.push_back(aimesh->mVertices[v].x);
            mesh.vertices.push_back(aimesh->mVertices[v].y);
            mesh.vertices.push_back(aimesh->mVertices[v].z);

            if (aimesh->HasTextureCoords(0)) {
                mesh.texCoords.push_back(aimesh->mTextureCoords[0][v].x);
                mesh.texCoords.push_back(aimesh->mTextureCoords[0][v].y);
            }
            else {
                mesh.texCoords.push_back(0.0f);
                mesh.texCoords.push_back(0.0f);
            }
        }

        for (unsigned int f = 0; f < aimesh->mNumFaces; f++) {
            const aiFace& face = aimesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                mesh.indices.push_back(face.mIndices[j]);
            }
        }

        meshes.push_back(mesh);
    }
}

static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);
}

void loadTexture(const char* filename) {

    int width, height, channels;

    unsigned char* imageData = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    
    if (imageData == nullptr) {
        std::cerr << "Error: No se pudo cargar la textura " << filename << std::endl;
        return;
    }
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    stbi_image_free(imageData);
}

void MoveCamera(const Uint8* keystate) {
    float adjustedSpeed = cameraSpeed;

    // Calcular la dirección de vista de la cámara usando los ángulos de órbita
    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    // Calcular los vectores de dirección
    glm::vec3 forward(
        cos(verticalRad) * sin(horizontalRad),
        sin(verticalRad),
        cos(verticalRad) * cos(horizontalRad)
    );

    // Calcular el vector derecho (perpendicular a forward y up)
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Calcular el vector up real
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

void MoveCameraWithMouse(int xrel, int yrel) {
    float sensitivity = cameraSpeed * 0.5f; // Ajusta este valor según necesites


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



void RotateCamera(int xrel, int yrel) {
    const float sensitivity = 0.3f;
    orbitAngleHorizontal += xrel * sensitivity;
    orbitAngleVertical = glm::clamp(orbitAngleVertical + yrel * sensitivity, -89.0f, 89.0f);


    float horizontalRad = glm::radians(orbitAngleHorizontal);
    float verticalRad = glm::radians(orbitAngleVertical);

    cameraX = targetX + orbitRadius * cos(verticalRad) * sin(horizontalRad);
    cameraY = targetY + orbitRadius * sin(verticalRad);
    cameraZ = targetZ + orbitRadius * cos(verticalRad) * cos(horizontalRad);
}

void FocusOnObject() {

    targetX = 0.0f;
    targetY = 0.0f;
    targetZ = 0.0f;

    orbitAngleHorizontal = 0.0f;
    orbitAngleVertical = 30.0f;

    RotateCamera(0, 0);
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

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Nueva configuración de la cámara
    gluLookAt(cameraX, cameraY, cameraZ,  // Posición de la cámara
        targetX, targetY, targetZ,   // Punto al que mira
        0.0f, 1.0f, 0.0f);          // Vector "up"

    // El resto del código de render permanece igual...
    drawGrid(10.0f, 20);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(0.1f, 0.1f, 0.1f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    // Renderizado del modelo...
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    for (const auto& mesh : meshes) {
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
bool MyWindow::processEvents(IEventProcessor* event_processor) {
    SDL_Event event;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        if (event_processor) event_processor->processEvent(event);

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
            else
            {
                !movingCameraWithMouse;
            }

            if (event.button.button == SDL_BUTTON_LEFT && keystate[SDL_SCANCODE_LCTRL])
            {
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
            }if (movingCamera)
            {
                MoveCamera(keystate);
            }if (movingCameraWithMouse)
            {
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
            RotateCamera(0, 0); // Actualiza la posición de la cámara
            break;

        case SDL_DROPFILE: {
            char* dropped_filedir = event.drop.file;
            std::cout << "Archivo arrastrado: " << dropped_filedir << std::endl;

            loadFBX(dropped_filedir);
            break;
        }
        default:
            ImGui_ImplSDL2_ProcessEvent(&event);
            break;
        }
    }

    return true;
}
int main(int argc, char** argv) {
    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

    init_openGL();
    loadTexture(filetex);
    loadFBX(filefbx);

    while (window.processEvents() && window.isOpen()) {
        const auto t0 = hrclock::now();

        render();

        window.swapBuffers();
        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
    }

    return 0;
}