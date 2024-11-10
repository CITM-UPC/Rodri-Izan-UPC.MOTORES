#pragma once
#include <GL/glew.h>
#include <string>
#include <stdexcept>

// Implementacion de la libreria stb_image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Clase para manejar texturas en OpenGL
class Texture {
public:
    //Constructor 
    Texture() = default;

    //Destructor
    ~Texture() {
        Cleanup();
    }

    // Bind de la textura
    void Bind(GLuint unit = 0) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    // UnBind de la textura
    void Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint GetID() const { return textureID; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetChannels() const { return channels; }

private:
    // CleanUp de la textura
    void Cleanup() {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
        }
    }

    GLuint textureID = 0; // ID de la textura
    int width = 0; // Ancho de la textura
    int height = 0; // Alto de la textura
    int channels = 0; // Canales de la textura
};