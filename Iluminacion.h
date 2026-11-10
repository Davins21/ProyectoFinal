
#ifndef ILUMINACION_H
#define ILUMINACION_H

#include <glad/glad.h>
#include <glm/glm.hpp>

class SunLight
{
private:
    glm::vec3 direction;
    glm::vec3 color;

    float ambientStrength;
    float speed;
    float elapsedTime;

    // Luz localizada
    glm::vec3 pointPosition;
    glm::vec3 pointColor;
    float pointIntensity;
    float pointRadius;

    GLint directionLoc;
    GLint colorLoc;
    GLint ambientLoc;

    GLint pointPositionLoc;
    GLint pointColorLoc;
    GLint pointIntensityLoc;
    GLint pointRadiusLoc;

public:
    SunLight(GLuint shaderID);

    void Update(float deltaTime);
    void Apply();
};

#endif
