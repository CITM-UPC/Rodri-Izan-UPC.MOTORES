#pragma once
#include "GameObject.h"

class Empty : public GameObject {
public:
    Empty(const std::string& name = "Empty") : GameObject(name) {}

    // Puedes a�adir funcionalidad espec�fica para los Emptys aqu�
    // Por ejemplo, un icono especial o comportamiento espec�fico
};
