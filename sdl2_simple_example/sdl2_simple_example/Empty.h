#pragma once
#include "GameObject.h"

class Empty : public GameObject {
public:
    Empty(const std::string& name = "Empty") : GameObject(name) {}

    // Puedes añadir funcionalidad específica para los Emptys aquí
    // Por ejemplo, un icono especial o comportamiento específico
};
