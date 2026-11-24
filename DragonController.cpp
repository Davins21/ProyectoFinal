
#include "DragonController.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// =============================================
// CONSTRUCTOR
// =============================================

DragonController::DragonController(
    AnimatedModelGLTF& dragon,
    BezierPath& path,
    float animationStart,
    float animationEnd,
    float flightDuration,
    float initialScale
)
    : dragon(dragon),
    path(path)
{
    this->animationStart = animationStart;
    this->animationEnd = animationEnd;

    this->flightDuration = flightDuration;
    this->flightTime = 0.0f;

    this->flightSpeed = 1.0f;
    this->dragonScale = initialScale;

    this->flightPaused = false;

    // Iniciar segmento de vuelo
    dragon.SetAnimationRange(
        animationStart,
        animationEnd,
        true
    );
}


// =============================================
// ACTUALIZAR MOVIMIENTO Y ANIMACION
// =============================================

void DragonController::Update(float deltaTime)
{
    // Actualizar animacion glTF de las alas
    dragon.Update(deltaTime);

    // Actualizar recorrido Bezier
    if (!flightPaused)
    {
        flightTime += deltaTime * flightSpeed;
    }
}


// =============================================
// MATRIZ DE TRANSFORMACION
// =============================================

glm::mat4 DragonController::GetModelMatrix() const
{
    // Obtener progreso circular
    float t = std::fmod(
        flightTime / flightDuration,
        1.0f
    );

    // Obtener posicion y direccion
    glm::vec3 position = path.GetPosition(t);
    glm::vec3 tangent = path.GetTangent(t);

    // Calcular orientacion horizontal
    float yaw = std::atan2(
        tangent.x,
        tangent.z
    );

    // Crear matriz del modelo
    glm::mat4 model(1.0f);

    // Aplicar posicion
    model = glm::translate(
        model,
        position
    );

    // Aplicar rotacion
    model = glm::rotate(
        model,
        yaw,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Aplicar escala
    model = glm::scale(
        model,
        glm::vec3(dragonScale)
    );

    return model;
}


// =============================================
// CONTROL DEL VUELO
// =============================================

void DragonController::SetFlightPaused(bool paused)
{
    flightPaused = paused;
}

bool DragonController::IsFlightPaused() const
{
    return flightPaused;
}


// =============================================
// CONTROL DE LAS ALAS
// =============================================

void DragonController::SetWingsPaused(bool paused)
{
    dragon.SetPaused(paused);
}

bool DragonController::AreWingsPaused() const
{
    return dragon.IsPaused();
}


// =============================================
// CONTROL DE VELOCIDAD
// =============================================

void DragonController::SetFlightSpeed(float speed)
{
    flightSpeed = glm::clamp(speed, 0.1f, 3.0f);
}

float DragonController::GetFlightSpeed() const
{
    return flightSpeed;
}


// =============================================
// CONTROL DE ESCALA
// =============================================

void DragonController::SetScale(float scale)
{
    dragonScale = glm::clamp(scale, 0.005f, 0.08f);
}

float DragonController::GetScale() const
{
    return dragonScale;
}


// =============================================
// REINICIAR RECORRIDO
// =============================================

void DragonController::Reset()
{
    flightTime = 0.0f;

    dragon.SetAnimationRange(
        animationStart,
        animationEnd,
        true
    );

    flightPaused = false;
    dragon.SetPaused(false);
}


// =============================================
// CONSULTAR TIEMPO DE VUELO
// =============================================

float DragonController::GetFlightTime() const
{
    return flightTime;
}
