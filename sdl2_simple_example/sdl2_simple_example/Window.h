#pragma once
#include <imgui.h>

class Window {
public:
    Window(const char* title) : title(title) {}
    virtual ~Window() = default;

    virtual void Render() {
        ImGui::Begin(title);
        DrawContent();
        ImGui::End();
    }

protected:
    virtual void DrawContent() = 0;
    const char* title;
};
