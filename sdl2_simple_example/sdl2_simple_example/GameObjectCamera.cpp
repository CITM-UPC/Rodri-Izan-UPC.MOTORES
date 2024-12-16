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
    // Implementaci�n de movimiento para una c�mara de GameObject
    // Similar a SceneCamera, pero con posibilidades de personalizaci�n
    // Puedes ajustar seg�n tus necesidades espec�ficas
}

void GameObjectCamera::Rotate(int xrel, int yrel) {
    // Implementaci�n de rotaci�n para una c�mara de GameObject
    // Puede ser diferente de SceneCamera
}

void GameObjectCamera::FocusOnObject() {
    // Implementaci�n de enfoque para una c�mara de GameObject
    // Podr�a enfocarse en el objeto asociado
}

void GameObjectCamera::SetPerspective(bool perspective) {
    isPerspective = perspective;
}

bool GameObjectCamera::IsPerspective() const {
    return isPerspective;
}