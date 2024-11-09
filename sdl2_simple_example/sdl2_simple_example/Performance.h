// PerformanceWindow.h
#pragma once
#include <chrono>
#include <vector>

class PerformanceWindow {
public:
    PerformanceWindow();
    void DrawPerformanceWindow();

private:
    static const int MAX_FPS_SAMPLES = 100; // Para mostrar un gráfico de historial
    std::vector<float> fpsHistory;  // Cambiado de deque a vector
    std::chrono::steady_clock::time_point lastFrameTime;
    float currentFps;

    void UpdateFPS();
    size_t GetProcessMemoryUsage() const;
    std::string FormatMemorySize(size_t bytes) const;
};