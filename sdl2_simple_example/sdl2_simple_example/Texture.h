#pragma once
#include <GL/glew.h>
#include <string>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class Texture {
public:
    Texture() = default;

    ~Texture() {
        Cleanup();
    }

    // Load texture from file
    bool LoadFromFile(const std::string& filepath) {
        // Clean up existing texture if any
        Cleanup();

        // Load image using stb_image
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (!data) {
            return false;
        }

        // Generate and bind OpenGL texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data and generate mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free image data
        stbi_image_free(data);

        return true;
    }

    // Create texture from raw data
    bool LoadFromMemory(unsigned char* data, int width, int height, int channels) {
        if (!data || width <= 0 || height <= 0) {
            return false;
        }

        Cleanup();

        this->width = width;
        this->height = height;
        this->channels = channels;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        return true;
    }

    void Bind(GLuint unit = 0) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint GetID() const { return textureID; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetChannels() const { return channels; }

private:
    void Cleanup() {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
        }
    }

    GLuint textureID = 0;
    int width = 0;
    int height = 0;
    int channels = 0;
};