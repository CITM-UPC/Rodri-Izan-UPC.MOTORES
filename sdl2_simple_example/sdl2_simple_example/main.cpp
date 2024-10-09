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

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

GLfloat cameraX = 10.0;
GLfloat cameraY = 40.0;
GLfloat cameraZ = 10.0;

// Cargar modelo FBX
const char* file = "C:/Users/rodrigoam/Documents/GitHub/Rodri-Izan.-MotoresUpc/Assets/cubo.fbx";


struct Mesh {
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};

std::vector<Mesh> meshes;

// Función para cargar un modelo OBJ utilizando Assimp
void loadFBX(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error al cargar el modelo: " << importer.GetErrorString() << std::endl;
        return;
    }
};


static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);
}

static void draw_triangle(const u8vec4& color, const vec3& center, double size) {
}


static void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_triangle(u8vec4(255, 0, 0, 255), vec3(0.0, 0.0, 0.0), 0.5);
}

void Draw(const char* object)
{
    const struct aiScene* scene = aiImportFile(object, aiProcess_Triangulate);

    if (!scene)
    {
        fprintf(stderr, "error: %s\n", aiGetErrorString());
        return;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        Mesh mesh;
        const aiMesh* aimesh = scene->mMeshes[i];


        //Vertex
        for (unsigned int v = 0; v < aimesh->mNumVertices; v++)
        {
            mesh.vertices.push_back(aimesh->mVertices[v].x);
            mesh.vertices.push_back(aimesh->mVertices[v].y);
            mesh.vertices.push_back(aimesh->mVertices[v].z);
        }

        for (unsigned int v = 0; v < aimesh->mNumVertices; v++)
        {
            const aiFace& face = aimesh->mFaces[v];
            for (unsigned int i = 0; i < face.mNumIndices; i++)
            {
                mesh.indices.push_back(face.mIndices[i]);
            }
        }

        meshes.push_back(mesh);
    }
}

// Función de inicialización de OpenGL
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

   

}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configurar la cámara
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);
    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    

    // Renderizar cada malla del modelo
    for (const auto& mesh : meshes) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, mesh.vertices.data());

        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, mesh.indices.data());

        glDisableClientState(GL_VERTEX_ARRAY);
    }
}


int main(int argc, char** argv) {
    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

    init_openGL();

    while (window.processEvents() && window.isOpen()) {
        const auto t0 = hrclock::now();
        display_func();
        window.swapBuffers();
        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
    }

    Draw(file);

    return 0;
}

