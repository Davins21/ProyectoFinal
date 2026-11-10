
#include "Iluminacion.h"
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

SunLight::SunLight(GLuint shaderID)
{
    // Luz solar general
    direction = glm::vec3(0.0f, 1.0f, 0.3f);
    color = glm::vec3(1.25f, 1.15f, 1.0f);

    ambientStrength = 0.25f;

    speed = 0.35f;
    elapsedTime = 0.0f;

    // Luz concentrada
    pointPosition = glm::vec3(0.0f, 8.0f, 0.0f);
    pointColor = glm::vec3(1.0f, 0.75f, 0.45f);

    pointIntensity = 5.0f;
    pointRadius = 20.0f;

    // Uniforms del sol
    directionLoc = glGetUniformLocation(shaderID, "sunDirection");
    colorLoc = glGetUniformLocation(shaderID, "sunColor");
    ambientLoc = glGetUniformLocation(shaderID, "ambientStrength");

    // Uniforms de la luz localizada
    pointPositionLoc = glGetUniformLocation(shaderID, "pointLightPosition");
    pointColorLoc = glGetUniformLocation(shaderID, "pointLightColor");
    pointIntensityLoc = glGetUniformLocation(shaderID, "pointLightIntensity");
    pointRadiusLoc = glGetUniformLocation(shaderID, "pointLightRadius");
}

void SunLight::Update(float deltaTime)
{
    elapsedTime += deltaTime * speed;

    float angle = sin(elapsedTime) * 1.15f;

    direction = glm::normalize(
        glm::vec3(
            sin(angle),
            0.75f + 0.15f * cos(angle),
            0.35f
        )
    );
}

void SunLight::Apply()
{
    glUniform3fv(directionLoc, 1, glm::value_ptr(direction));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));
    glUniform1f(ambientLoc, ambientStrength);

    glUniform3fv(pointPositionLoc, 1, glm::value_ptr(pointPosition));
    glUniform3fv(pointColorLoc, 1, glm::value_ptr(pointColor));

    glUniform1f(pointIntensityLoc, pointIntensity);
    glUniform1f(pointRadiusLoc, pointRadius);
}
