
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderClass.h"

class Camera
{
public:

    int width;
    int height;

    glm::vec3 Position;
    glm::vec3 Orientation;
    glm::vec3 Up;

    float speed;
    float sensitivity;

    float yaw;
    float pitch;

    float lastX;
    float lastY;

    bool firstMouse;

    // Inicializar camara
    Camera(int width, int height, glm::vec3 position);

    // Configurar entradas
    void SetupInputs(GLFWwindow* window);

    // Procesar movimiento de camara
    void ProcessInputs(GLFWwindow* window, float deltaTime);

    // Procesar movimiento del mouse
    void ProcessMouseMovement(
        float xoffset,
        float yoffset,
        GLboolean constrainPitch = true
    );

    // Cambiar entre camara 3D e interfaz
    void SetCameraMode(GLFWwindow* window, bool enabled);

    // Obtener matriz de vista
    glm::mat4 GetViewMatrix();

    // Obtener matriz de proyeccion
    glm::mat4 GetProjectionMatrix(
        float FOVdeg,
        float nearPlane,
        float farPlane
    );

    // Actualizar matriz MVP
    void UpdateMatrix(
        Shader& shader,
        const char* uniformName,
        glm::mat4 modelMatrix,
        float FOVdeg,
        float nearPlane,
        float farPlane
    );

private:

    // Estado del modo de camara
    bool cameraMode = true;

    // Callback del mouse
    static void MouseCallback(
        GLFWwindow* window,
        double xposIn,
        double yposIn
    );
};

#endif
