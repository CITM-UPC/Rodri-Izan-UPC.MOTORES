#include "Console.h"
#include <iostream>

Console::Console() {
    RedirectOutput();  // Redirige std::cout a esta consola al crearla
}

void Console::Draw(const std::string& title) {
    ImGui::Begin(title.c_str());

    if (ImGui::Button("Clear")) {
        Clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Scroll to Bottom")) {
        scrollToBottom = true;
    }

    ImGui::Separator();

    ImGui::BeginChild("ConsoleScroll");
    for (const auto& log : logs) {
        ImGui::TextUnformatted(log.c_str());
    }

    if (scrollToBottom) {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;
    }
    ImGui::EndChild();

    ImGui::End();
}

void Console::AddLog(const std::string& message) {
    logs.push_back(message);
    scrollToBottom = true;
}

void Console::RedirectOutput() {
    // Redirige std::cout a un buffer
    std::cout.rdbuf(buffer.rdbuf());
}

void Console::Clear() {
    logs.clear();
}

// Captura el contenido del buffer y lo pasa al vector de logs
void Console::Flush() {
    std::string captured = buffer.str();
    if (!captured.empty()) {
        logs.push_back(captured);
        buffer.str("");  // Limpia el buffer
        buffer.clear();
    }
}
