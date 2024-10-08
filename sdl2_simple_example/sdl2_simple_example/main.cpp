#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include <imgui_impl_sdl2.h>

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

GLfloat v0[] = { -0.1, -0.1,  0.1 };  // Front-bottom-left
GLfloat v1[] = { 0.1, -0.1,  0.1 };  // Front-bottom-right
GLfloat v2[] = { 0.1,  0.1,  0.1 };  // Front-top-right
GLfloat v3[] = { -0.1,  0.1,  0.1 };  // Front-top-left
GLfloat v4[] = { -0.1, -0.1, -0.1 };  // Back-bottom-left
GLfloat v5[] = { 0.1, -0.1, -0.1 };  // Back-bottom-right
GLfloat v6[] = { 0.1,  0.1, -0.1 };  // Back-top-right
GLfloat v7[] = { -0.1,  0.1, -0.1 };  // Back-top-left

GLfloat angle = 1.0f;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);
}

static void draw_triangle(const u8vec4& color, const vec3& center, double size) {
	//Dibuixa triangle
	//glColor4ub(color.r, color.g, color.b, color.a);
	//glBegin(GL_TRIANGLES);
	//glVertex3d(center.x, center.y + size, center.z);
	//glVertex3d(center.x - size, center.y - size, center.z);
	//glVertex3d(center.x + size, center.y - size, center.z);
	//glEnd();

	//// Dibuixar una línea 10 unitats amunt
	//glLineWidth(2.0f);
	//glBegin(GL_LINES);
	//glVertex3f(0.f, 0.f, 0.f);
	//glVertex3f(0.f, 10.f, 0.f);
	//glEnd();

    glRotatef(angle, 1.0f, 1.0f, 0.0f);

    glBegin(GL_TRIANGLES);

    //Front face
    glVertex3fv(v0);    // v0-v1-v2
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v2);    // v2-v3-v0
    glVertex3fv(v3);
    glVertex3fv(v0);

    // Right face
    glVertex3fv(v1);    // v1-v5-v6
    glVertex3fv(v5);
    glVertex3fv(v6);
    glVertex3fv(v6);    // v6-v2-v1
    glVertex3fv(v2);
    glVertex3fv(v1);

    // Back face
    glVertex3fv(v5);    // v5-v4-v7
    glVertex3fv(v4);
    glVertex3fv(v7);
    glVertex3fv(v7);    // v7-v6-v5
    glVertex3fv(v6);
    glVertex3fv(v5);


    // Left face
    glVertex3fv(v4);    // v4-v0-v3
    glVertex3fv(v0);
    glVertex3fv(v3);
    glVertex3fv(v3);    // v3-v7-v4
    glVertex3fv(v7);
    glVertex3fv(v4);


    // Top face
    glVertex3fv(v3);    // v3-v2-v6
    glVertex3fv(v2);
    glVertex3fv(v6);
    glVertex3fv(v6);    // v6-v7-v3
    glVertex3fv(v7);
    glVertex3fv(v3);


    // Bottom face
    glVertex3fv(v4);    // v4-v5-v1
    glVertex3fv(v5);
    glVertex3fv(v1);
    glVertex3fv(v1);    // v1-v0-v4
    glVertex3fv(v0);
    glVertex3fv(v4);


    glEnd();
}


static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw_triangle(u8vec4(255, 0, 0, 255), vec3(0.0, 0.0, 0.0), 0.5);
}

int main(int argc, char** argv) {
	MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

	init_openGL();

	while(window.processEvents() && window.isOpen()) {
		const auto t0 = hrclock::now();
		display_func();
		window.swapBuffers();
		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if(dt<FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}

	return 0;
}