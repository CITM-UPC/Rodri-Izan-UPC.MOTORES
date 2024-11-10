#pragma once
#include <chrono>
#include <vector>

// Clase para mostrar el rendimiento de la aplicaci�n
class PerformanceWindow {
public:
    PerformanceWindow(); // Constructor
    void DrawPerformanceWindow(); // Dibujar la ventana de rendimiento

private:
    static const int MAX_FPS_SAMPLES = 100; // Para mostrar un gr�fico de historial
    std::vector<float> fpsHistory; // Historial de FPS
    std::chrono::steady_clock::time_point lastFrameTime; // Para calcular el FPS
    float currentFps; // FPS actual

    // Actualizar el FPS
    void UpdateFPS();

    // Obtener el uso de memoria del proceso
    size_t GetProcessMemoryUsage() const;

    // Formatear el tama�o de la memoria
    std::string FormatMemorySize(size_t bytes) const;
};