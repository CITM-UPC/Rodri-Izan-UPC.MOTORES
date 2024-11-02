#pragma once
// EditScene.h
#ifndef EDIT_SCENE_H
#define EDIT_SCENE_H

// Declaraciones de variables globales
extern int selectedGameObject;
extern bool isActive;

// Declaraciones de funciones
void DrawHierarchyWindow();
void DrawInspectorWindow();
void DrawAssetWindow();
void RenderEditor();

#endif // EDIT_SCENE_H

