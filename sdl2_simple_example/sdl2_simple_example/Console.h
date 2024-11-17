#pragma once

#include <vector>
#include <string>
#include <sstream>
#include "imgui.h"

class Console {
public:
    Console();
    void Draw(const std::string& title);   // Dibuja la consola
    void AddLog(const std::string& message); // Agrega un log directamente
    void RedirectOutput();  // Redirige std::cout a esta consola
    void Clear();  // Limpia los logs

    void Flush();

private:
    std::vector<std::string> logs;  // Almacena los mensajes de la consola
    std::ostringstream buffer;      // Buffer para redirigir std::cout
    bool scrollToBottom = true;     // Indica si se debe hacer scroll automático
};
