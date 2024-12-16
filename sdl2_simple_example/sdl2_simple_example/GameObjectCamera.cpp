#include "GameObjectCamera.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdexcept>

GameObjectCamera::GameObjectCamera()
    : Camera(), isPerspective(true)
{
}

void GameObjectCamera::Move(const Uint8* keystate) {
    // Implementación de movimiento para una cámara de GameObject
    // Similar a SceneCamera, pero con posibilidades de personalización
    // Puedes ajustar según tus necesidades específicas
}

void GameObjectCamera::Rotate(int xrel, int yrel) {
    // Implementación de rotación para una cámara de GameObject
    // Puede ser diferente de SceneCamera
}

void GameObjectCamera::FocusOnObject() {
    // Implementación de enfoque para una cámara de GameObject
    // Podría enfocarse en el objeto asociado
}

void GameObjectCamera::SetPerspective(bool perspective) {
    isPerspective = perspective;
}

bool GameObjectCamera::IsPerspective() const {
    return isPerspective;
}