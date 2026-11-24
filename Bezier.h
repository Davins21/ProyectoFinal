
#ifndef BEZIER_H
#define BEZIER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

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

    // Calcular posicion de la curva
    glm::vec3 GetPosition(float t) const;

    // Calcular direccion de la curva
    glm::vec3 GetTangent(float t) const;
};


// =============================================
// CLASE PARA ADMINISTRAR LA TRAYECTORIA
// =============================================

class BezierPath
{
public:

    // Crear recorrido circular
    BezierPath(
        glm::vec3 center,
        float radius,
        float height,
        int segments = 80
    );

    // Obtener posicion del recorrido completo
    glm::vec3 GetPosition(float t) const;

    // Obtener direccion del recorrido completo
    glm::vec3 GetTangent(float t) const;

    // Dibujar trayectoria
    void Draw(
        GLuint shaderProgram,
        const glm::mat4& viewProjection,
        bool visible
    ) const;

    // Liberar recursos OpenGL
    void Delete();

private:

    std::vector<Bezier> curves;
    std::vector<glm::vec3> points;

    GLuint VAO = 0;
    GLuint VBO = 0;

    int segments;

    // Crear puntos y buffers
    void CreatePath();
};

#endif
