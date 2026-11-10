#ifndef BEZIER_H
#define BEZIER_H

#include <glm/glm.hpp>

class Bezier
{
public:

    glm::vec3 P0, P1, P2, P3;

    Bezier(
        glm::vec3 p0,
        glm::vec3 p1,
        glm::vec3 p2,
        glm::vec3 p3
    );

    glm::vec3 GetPosition(float t) const;

    glm::vec3 GetTangent(float t) const;
};

#endif
