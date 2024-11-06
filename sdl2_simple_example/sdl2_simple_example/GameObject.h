#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"

class GameObject {
public:
    // Constructor
    GameObject(Mesh* mesh, Texture* texture, const Transform& transform = Transform())
        : mesh(mesh), texture(texture), transform(transform) {}

    // Renderizar el GameObject
    void Render() const {
        if (texture) {
            texture->Bind(); // Enlaza la textura al objeto
        }

        // Aplicar la matriz de transformación
        glPushMatrix();
        glMultMatrixf(&transform.GetMatrix()[0][0]); // Multiplica la matriz actual por la matriz de transformación

        // Renderizar la malla
        if (mesh) {
            glBindVertexArray(mesh->VAO);
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glPopMatrix();

        if (texture) {
            texture->Unbind();
        }
    }

    // Getters y Setters
    Transform& GetTransform() { return transform; }
    const Transform& GetTransform() const { return transform; }

    Mesh* mesh;
    Texture* texture;
    Transform transform;
};
