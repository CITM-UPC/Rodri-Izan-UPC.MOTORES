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

#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

GLuint textureID;

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

GLfloat cameraX = 0.0f;
GLfloat cameraY = 10.0f; 
GLfloat cameraZ = 5.0f; 

GLfloat cameraAngleX = 0.0f;
GLfloat cameraAngleY = 0.0f;
GLfloat cameraAngleZ = 0.0f;
bool rotatingCamera = false;
bool movingCamera = false;

// Cargar modelo FBX
//const char* file = "C:/Users/rodrigoam/Documents/GitHub/Rodri-Izan-UPC.MOTORES/Assets/notdeletedcube.fbx";
const char* file = "C:/Users/G513/Documents/GitHub/Rodri-Izan-UPC.MOTORES/Assets/masterchief.fbx";

struct Mesh {
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> texCoords;  // Coordenadas de la textura
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

    // Recorrer las mallas
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh* aimesh = scene->mMeshes[i];
        std::cout << "Malla " << i << " cargada con " << aimesh->mNumVertices << " vértices." << std::endl;

        Mesh mesh;

        // Almacenar vértices
        for (unsigned int v = 0; v < aimesh->mNumVertices; v++) {
            mesh.vertices.push_back(aimesh->mVertices[v].x);
            mesh.vertices.push_back(aimesh->mVertices[v].y);
            mesh.vertices.push_back(aimesh->mVertices[v].z);

            // Almacenar coordenadas de textura si están disponibles
            if (aimesh->HasTextureCoords(0)) {  // Usa el primer conjunto de UV
                mesh.texCoords.push_back(aimesh->mTextureCoords[0][v].x);
                mesh.texCoords.push_back(aimesh->mTextureCoords[0][v].y);
            }
            else {
                // Si no tiene coordenadas de textura, usa (0, 0) como valor por defecto
                mesh.texCoords.push_back(0.0f);
                mesh.texCoords.push_back(0.0f);
            }
        }

        // Almacenar índices (caras)
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

void Texturegenerator() {
    GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
    for (int i = 0; i < CHECKERS_HEIGHT; i++) {
        for (int j = 0; j < CHECKERS_WIDTH; j++) {
            int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
            checkerImage[i][j][0] = (GLubyte)c;
            checkerImage[i][j][1] = (GLubyte)c;
            checkerImage[i][j][2] = (GLubyte)c;
            checkerImage[i][j][3] = (GLubyte)255;
        }
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
}

//void drawGrid(float size, float step) {
//    glBegin(GL_LINES);
//    glColor3f(0.5f, 0.5f, 0.5f); // Color gris para las líneas
//
//    for (float i = -size; i <= size; i += step) {
//        // Líneas paralelas al eje X
//        glVertex3f(i, 0, -size);
//        glVertex3f(i, 0, size);
//
//        // Líneas paralelas al eje Z
//        glVertex3f(-size, 0, i);
//        glVertex3f(size, 0, i);
//    }
//
//    glEnd();
//}


void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configurar la cámara
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);  // Campo de visión, aspecto, plano cercano y lejano

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);  // Posición de la cámara


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
     
    //Renderizar cada malla del modelo
    for (const auto& mesh : meshes) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, mesh.vertices.data());

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);  // Habilitar coordenadas de textura
        glTexCoordPointer(2, GL_FLOAT, 0, mesh.texCoords.data());  // Pasar coordenadas de textura

        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, mesh.indices.data());

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glDisable(GL_TEXTURE_2D);

    // Rotación de la cámara (añadir aquí)
    glRotatef(cameraAngleY, 1.0f, 0.0f, 0.0f); // Rotar en eje X
    glRotatef(cameraAngleX, 0.0f, 1.0f, 0.0f); // Rotar en eje Y
    glRotatef(cameraAngleZ, 0.0f, 0.0f, 1.0f); // Rotar en eje Z

    // Dibujar el suelo de rejillas
    /*drawGrid(50.0f, 1.0f);*/

    // Transformación del modelo
    glPushMatrix();  // Guardar la matriz actual
    glTranslatef(0.0f, 0.0f, 0.0f);  // Aquí puedes ajustar  X y Z
    glPopMatrix();


    glFlush();
}



int main(int argc, char** argv) {
    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

    init_openGL();

    Texturegenerator();

    loadFBX(file);

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
