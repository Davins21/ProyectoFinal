#pragma once

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "DragonController.h"

class Interfaz
{
private:

    bool showTrajectory;

public:

    Interfaz();

    // Inicializar ImGui
    void Initialize(GLFWwindow* window);

    // Iniciar un nuevo frame
    void BeginFrame();

    // Dibujar el panel de control
    void Draw(DragonController& dragonController);

    // Mostrar el resultado de ImGui
    void Render();

    // Consultar si ImGui utiliza el teclado
    bool WantsCaptureKeyboard();

    // Consultar si ImGui utiliza el mouse
    bool WantsCaptureMouse();

    // Consultar visibilidad de la trayectoria
    bool GetShowTrajectory();

    // Liberar recursos
    void Shutdown();
};
