#include "Bezier.h"

#include <glm/common.hpp>

// Constructor
Bezier::Bezier(
    glm::vec3 p0,
    glm::vec3 p1,
    glm::vec3 p2,
    glm::vec3 p3)
{
    P0 = p0;
    P1 = p1;
    P2 = p2;
    P3 = p3;
}

// Calcular posicion de la curva cubica
glm::vec3 Bezier::GetPosition(float t) const
{
    t = glm::clamp(t, 0.0f, 1.0f);

    float u = 1.0f - t;

    return
        (u * u * u) * P0 +
        (3.0f * u * u * t) * P1 +
        (3.0f * u * t * t) * P2 +
        (t * t * t) * P3;
}

// Calcular la tangente de la curva
glm::vec3 Bezier::GetTangent(float t) const
{
    t = glm::clamp(t, 0.0f, 1.0f);

    float u = 1.0f - t;

    return
        (3.0f * u * u) * (P1 - P0) +
        (6.0f * u * t) * (P2 - P1) +
        (3.0f * t * t) * (P3 - P2);
}
