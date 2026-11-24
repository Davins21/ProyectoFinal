
#include "Interfaz.h"

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"


// =============================================
// CONSTRUCTOR
// =============================================

Interfaz::Interfaz()
{
    showTrajectory = true;
}


// =============================================
// INICIALIZAR IMGUI
// =============================================

void Interfaz::Initialize(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init("#version 450");
}


// =============================================
// INICIAR FRAME
// =============================================

void Interfaz::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();

    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
}


// =============================================
// PANEL DE CONTROL DEL DRAGON
// =============================================

void Interfaz::Draw(DragonController& dragonController)
{
    ImGui::SetNextWindowSize(
        ImVec2(340, 0),
        ImGuiCond_FirstUseEver
    );

    ImGui::Begin("Mountain Dragon Controller");

    ImGui::Text("CASTILLO Y MOUNTAIN DRAGON");

    ImGui::Separator();


    // Control del vuelo

    ImGui::Text("Control del vuelo");

    if (dragonController.IsFlightPaused())
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.5f, 0.2f, 1.0f),
            "Vuelo pausado"
        );
    }
    else
    {
        ImGui::TextColored(
            ImVec4(0.2f, 1.0f, 0.3f, 1.0f),
            "Vuelo activo"
        );
    }

    if (ImGui::Button("Pausar vuelo", ImVec2(145, 35)))
    {
        dragonController.SetFlightPaused(true);
    }

    ImGui::SameLine();

    if (ImGui::Button("Reanudar vuelo", ImVec2(145, 35)))
    {
        dragonController.SetFlightPaused(false);
    }


    // Control de las alas

    ImGui::Separator();

    ImGui::Text("Control de las alas");

    if (dragonController.AreWingsPaused())
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.5f, 0.2f, 1.0f),
            "Animacion de alas pausada"
        );
    }
    else
    {
        ImGui::TextColored(
            ImVec4(0.2f, 1.0f, 0.3f, 1.0f),
            "Animacion de alas activa"
        );
    }

    if (ImGui::Button("Pausar alas", ImVec2(145, 35)))
    {
        dragonController.SetWingsPaused(true);
    }

    ImGui::SameLine();

    if (ImGui::Button("Activar alas", ImVec2(145, 35)))
    {
        dragonController.SetWingsPaused(false);
    }


    // Configuracion del movimiento

    ImGui::Separator();

    ImGui::Text("Configuracion del movimiento");

    float speedValue = dragonController.GetFlightSpeed();

    if (ImGui::SliderFloat(
        "Velocidad del vuelo",
        &speedValue,
        0.1f,
        3.0f,
        "%.1f"))
    {
        dragonController.SetFlightSpeed(speedValue);
    }


    // Escala del dragon

    float scaleValue = dragonController.GetScale();

    if (ImGui::SliderFloat(
        "Tamano del dragon",
        &scaleValue,
        0.005f,
        0.08f,
        "%.3f"))
    {
        dragonController.SetScale(scaleValue);
    }


    // Trayectoria Bezier

    ImGui::Separator();

    ImGui::Text("Trayectoria Bezier");

    ImGui::Checkbox(
        "Mostrar trayectoria",
        &showTrajectory
    );

    ImGui::Text(
        "Tiempo de vuelo: %.2f s",
        dragonController.GetFlightTime()
    );


    // Reiniciar recorrido

    ImGui::Separator();

    if (ImGui::Button("Reiniciar recorrido", ImVec2(-1, 35)))
    {
        dragonController.Reset();
    }


    // Controles de teclado

    ImGui::Text("Controles de teclado:");

    ImGui::BulletText("TAB: Camara / Interfaz");

    ImGui::BulletText("P: Pausar o continuar todo");

    ImGui::BulletText("R: Reiniciar recorrido");

    ImGui::BulletText("[: Retroceder animacion");

    ImGui::BulletText("]: Avanzar animacion");

    ImGui::End();
}


// =============================================
// RENDERIZAR INTERFAZ
// =============================================

void Interfaz::Render()
{
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(
        ImGui::GetDrawData()
    );
}


// =============================================
// CONSULTAR ENTRADAS DE IMGUI
// =============================================

bool Interfaz::WantsCaptureKeyboard()
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool Interfaz::WantsCaptureMouse()
{
    return ImGui::GetIO().WantCaptureMouse;
}


// =============================================
// VISIBILIDAD DE LA TRAYECTORIA
// =============================================

bool Interfaz::GetShowTrajectory()
{
    return showTrajectory;
}


// =============================================
// LIBERAR RECURSOS
// =============================================

void Interfaz::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
}
