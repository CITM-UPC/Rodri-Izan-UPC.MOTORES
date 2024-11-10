// PerformanceWindow.cpp
#include "Performance.h"
#include "imgui.h"
#include <format>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#endif

PerformanceWindow::PerformanceWindow()
    : currentFps(0.0f)
    , lastFrameTime(std::chrono::steady_clock::now()) {
    fpsHistory.reserve(MAX_FPS_SAMPLES);  // Reservar espacio para evitar realocaciones
}

void PerformanceWindow::UpdateFPS() {
    auto currentTime = std::chrono::steady_clock::now(); // Obtener el tiempo actual
    float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count(); // Calcular el tiempo transcurrido
    lastFrameTime = currentTime; // Actualizar el tiempo del último frame

    currentFps = 1.0f / deltaTime;

    // Mantener un historial de FPS para el gráfico
    if (fpsHistory.size() >= MAX_FPS_SAMPLES) {
        fpsHistory.erase(fpsHistory.begin());
    }
    fpsHistory.push_back(currentFps);
}

size_t PerformanceWindow::GetProcessMemoryUsage() const {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss * 1024;
    }
#endif
    return 0;
}

std::string PerformanceWindow::FormatMemorySize(size_t bytes) const {
    const char* units[] = { "B", "KB", "MB", "GB" };
    int unitIndex = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unitIndex < 3) {
        size /= 1024.0;
        unitIndex++;
    }

    return std::format("{:.2f} {}", size, units[unitIndex]);
}

void PerformanceWindow::DrawPerformanceWindow() {
    // Llamar a UpdateFPS para calcular el FPS actual y actualizar el historial
    UpdateFPS();

    ImGui::Begin("Performance Monitor");

    ImGui::Text("FPS: %.1f", currentFps);

    // Gráfico de FPS
    if (!fpsHistory.empty()) {
        ImGui::PlotLines("FPS History",
            fpsHistory.data(),
            static_cast<int>(fpsHistory.size()),
            0, nullptr,
            0.0f, 150.0f,
            ImVec2(0, 80));
    }


    // Mostrar uso de memoria
    size_t memoryUsage = GetProcessMemoryUsage();
    ImGui::Text("Memory Usage: %s", FormatMemorySize(memoryUsage).c_str());

    // Barra de progreso para memoria
    float memoryGB = static_cast<float>(memoryUsage) / (1024.0f * 1024.0f * 1024.0f);
    ImGui::ProgressBar(memoryGB / 8.0f, ImVec2(-1, 0), FormatMemorySize(memoryUsage).c_str());

    ImGui::End();
}