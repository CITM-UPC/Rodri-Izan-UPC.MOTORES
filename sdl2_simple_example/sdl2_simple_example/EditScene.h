#pragma once
#include "Hierarchy.h"
#include "Inspector.h"
#include "Transform.h"

class EditScene {
public:
    EditScene();
    void RenderEditor();

private:
    Hierarchy hierarchy;
    Inspector inspector;
    Transform assets;
};