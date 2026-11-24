
#include "Bezier.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include <cmath>

// =============================================
// CONSTRUCTOR DE BEZIER
// =============================================

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


// =============================================
// POSICION DE LA CURVA CUBICA
// =============================================

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


// =============================================
// TANGENTE DE LA CURVA
// =============================================

glm::vec3 Bezier::GetTangent(float t) const
{
    t = glm::clamp(t, 0.0f, 1.0f);

    float u = 1.0f - t;

    return
        (3.0f * u * u) * (P1 - P0) +
        (6.0f * u * t) * (P2 - P1) +
        (3.0f * t * t) * (P3 - P2);
}


// =============================================
// CONSTRUCTOR DE LA TRAYECTORIA COMPLETA
// =============================================

BezierPath::BezierPath(
    glm::vec3 center,
    float radius,
    float height,
    int segments)
{
    this->segments = segments;

    const float k = 0.55228475f;

    float r = radius;
    float h = height;

    // Curva 1: frente hacia derecha

    curves.emplace_back(
        center + glm::vec3(0, h, r),
        center + glm::vec3(k * r, h, r),
        center + glm::vec3(r, h, k * r),
        center + glm::vec3(r, h, 0)
    );

    // Curva 2: derecha hacia atras

    curves.emplace_back(
        center + glm::vec3(r, h, 0),
        center + glm::vec3(r, h, -k * r),
        center + glm::vec3(k * r, h, -r),
        center + glm::vec3(0, h, -r)
    );

    // Curva 3: atras hacia izquierda

    curves.emplace_back(
        center + glm::vec3(0, h, -r),
        center + glm::vec3(-k * r, h, -r),
        center + glm::vec3(-r, h, -k * r),
        center + glm::vec3(-r, h, 0)
    );

    // Curva 4: izquierda hacia frente

    curves.emplace_back(
        center + glm::vec3(-r, h, 0),
        center + glm::vec3(-r, h, k * r),
        center + glm::vec3(-k * r, h, r),
        center + glm::vec3(0, h, r)
    );

    CreatePath();
}


// =============================================
// GENERAR PUNTOS Y BUFFERS DE OPENGL
// =============================================

void BezierPath::CreatePath()
{
    points.clear();

    // Generar los puntos de las cuatro curvas

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j <= segments; j++)
        {
            float t = static_cast<float>(j) / segments;

            points.push_back(
                curves[i].GetPosition(t)
            );
        }
    }

    // Crear VAO y VBO

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        points.size() * sizeof(glm::vec3),
        points.data(),
        GL_STATIC_DRAW
    );

    // Posicion de cada vertice

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec3),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


// =============================================
// POSICION DEL RECORRIDO COMPLETO
// =============================================

glm::vec3 BezierPath::GetPosition(float t) const
{
    t = glm::clamp(t, 0.0f, 1.0f);

    int index = static_cast<int>(t * 4.0f);

    if (index > 3)
        index = 3;

    float localT = t * 4.0f - index;

    return curves[index].GetPosition(localT);
}


// =============================================
// DIRECCION DEL RECORRIDO COMPLETO
// =============================================

glm::vec3 BezierPath::GetTangent(float t) const
{
    t = glm::clamp(t, 0.0f, 1.0f);

    int index = static_cast<int>(t * 4.0f);

    if (index > 3)
        index = 3;

    float localT = t * 4.0f - index;

    return curves[index].GetTangent(localT);
}


// =============================================
// DIBUJAR LA TRAYECTORIA
// =============================================

void BezierPath::Draw(
    GLuint shaderProgram,
    const glm::mat4& viewProjection,
    bool visible) const
{
    if (!visible || VAO == 0)
        return;

    glUseProgram(shaderProgram);

    // Matriz de camara

    GLint viewLoc = glGetUniformLocation(
        shaderProgram,
        "uViewProjection"
    );

    glUniformMatrix4fv(
        viewLoc,
        1,
        GL_FALSE,
        glm::value_ptr(viewProjection)
    );

    // Color de la trayectoria

    GLint colorLoc = glGetUniformLocation(
        shaderProgram,
        "curveColor"
    );

    glUniform3f(
        colorLoc,
        0.0f,
        0.85f,
        1.0f
    );

    glBindVertexArray(VAO);

    // Mantener la prueba de profundidad para
    // que el castillo oculte la parte trasera.

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glLineWidth(3.0f);

    glDrawArrays(
        GL_LINE_STRIP,
        0,
        static_cast<GLsizei>(points.size())
    );

    glDepthMask(GL_TRUE);

    glBindVertexArray(0);
}


// =============================================
// LIBERAR RECURSOS
// =============================================

void BezierPath::Delete()
{
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    points.clear();
    curves.clear();
}
