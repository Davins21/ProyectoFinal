
#ifndef DRAGON_CONTROLLER_H
#define DRAGON_CONTROLLER_H

#include <glm/glm.hpp>
#include "AnimatedModelGLTF.h"
#include "Bezier.h"

class DragonController
{
public:

    // Constructor
    DragonController(
        AnimatedModelGLTF& dragon,
        BezierPath& path,
        float animationStart,
        float animationEnd,
        float flightDuration,
        float initialScale
    );

    // Actualizar movimiento y animacion
    void Update(float deltaTime);

    // Obtener matriz de transformacion
    glm::mat4 GetModelMatrix() const;

    // Control del vuelo
    void SetFlightPaused(bool paused);
    bool IsFlightPaused() const;

    // Control de las alas
    void SetWingsPaused(bool paused);
    bool AreWingsPaused() const;

    // Control de velocidad
    void SetFlightSpeed(float speed);
    float GetFlightSpeed() const;

    // Control de escala
    void SetScale(float scale);
    float GetScale() const;

    // Reiniciar recorrido
    void Reset();

    // Consultar tiempo de vuelo
    float GetFlightTime() const;

private:

    AnimatedModelGLTF& dragon;
    BezierPath& path;

    float animationStart;
    float animationEnd;

    float flightDuration;
    float flightTime;

    float flightSpeed;
    float dragonScale;

    bool flightPaused;
};

#endif
