#include "Camera.h"

// =============================================
// CONSTRUCTOR
// =============================================

Camera::Camera(int width, int height, glm::vec3 position)
{
    this->width = width;
    this->height = height;
    this->Position = position;

    this->Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    this->Up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Velocidad y sensibilidad
    this->speed = 10.0f;
    this->sensitivity = 0.1f;

    // Orientacion inicial
    this->yaw = -90.0f;
    this->pitch = 0.0f;

    // Posicion inicial del mouse
    this->firstMouse = true;
    this->lastX = width / 2.0f;
    this->lastY = height / 2.0f;
}


// =============================================
// CONFIGURAR ENTRADAS
// =============================================

void Camera::SetupInputs(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);

    glfwSetCursorPosCallback(
        window,
        Camera::MouseCallback
    );

    // Iniciar en modo camara
    SetCameraMode(window, true);
}


// =============================================
// CAMBIAR MODO DE CAMARA
// =============================================

void Camera::SetCameraMode(GLFWwindow* window, bool enabled)
{
    cameraMode = enabled;

    if (enabled)
    {
        // Capturar y ocultar cursor
        glfwSetInputMode(
            window,
            GLFW_CURSOR,
            GLFW_CURSOR_DISABLED
        );

        // Evitar saltos al volver a activar la camara
        firstMouse = true;
    }
    else
    {
        // Mostrar cursor para utilizar ImGui
        glfwSetInputMode(
            window,
            GLFW_CURSOR,
            GLFW_CURSOR_NORMAL
        );

        firstMouse = true;
    }
}


// =============================================
// CALLBACK DEL MOUSE
// =============================================

void Camera::MouseCallback(
    GLFWwindow* window,
    double xposIn,
    double yposIn
)
{
    Camera* camera = reinterpret_cast<Camera*>(
        glfwGetWindowUserPointer(window)
        );

    if (!camera || !camera->cameraMode)
        return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (camera->firstMouse)
    {
        camera->lastX = xpos;
        camera->lastY = ypos;

        camera->firstMouse = false;
    }

    float xoffset = xpos - camera->lastX;
    float yoffset = camera->lastY - ypos;

    camera->lastX = xpos;
    camera->lastY = ypos;

    camera->ProcessMouseMovement(
        xoffset,
        yoffset
    );
}


// =============================================
// MOVIMIENTO DE CAMARA
// =============================================

void Camera::ProcessInputs(GLFWwindow* window, float deltaTime)
{
    // No mover la camara en modo interfaz
    if (!cameraMode)
        return;

    float velocity = speed * deltaTime;

    // Adelante
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Position += velocity * Orientation;

    // Atras
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Position -= velocity * Orientation;

    // Izquierda
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Position -= glm::normalize(
            glm::cross(Orientation, Up)
        ) * velocity;

    // Derecha
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Position += glm::normalize(
            glm::cross(Orientation, Up)
        ) * velocity;

    // Subir
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        Position += velocity * Up;

    // Bajar
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        Position -= velocity * Up;
}


// =============================================
// ROTACION DE CAMARA CON MOUSE
// =============================================

void Camera::ProcessMouseMovement(
    float xoffset,
    float yoffset,
    GLboolean constrainPitch
)
{
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;

        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    glm::vec3 direction;

    direction.x =
        cos(glm::radians(yaw)) *
        cos(glm::radians(pitch));

    direction.y =
        sin(glm::radians(pitch));

    direction.z =
        sin(glm::radians(yaw)) *
        cos(glm::radians(pitch));

    Orientation = glm::normalize(direction);
}


// =============================================
// MATRIZ DE VISTA
// =============================================

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(
        Position,
        Position + Orientation,
        Up
    );
}


// =============================================
// MATRIZ DE PROYECCION
// =============================================

glm::mat4 Camera::GetProjectionMatrix(
    float FOVdeg,
    float nearPlane,
    float farPlane
)
{
    return glm::perspective(
        glm::radians(FOVdeg),
        static_cast<float>(width) / static_cast<float>(height),
        nearPlane,
        farPlane
    );
}


// =============================================
// ACTUALIZAR MATRIZ MVP
// =============================================

void Camera::UpdateMatrix(
    Shader& shader,
    const char* uniformName,
    glm::mat4 modelMatrix,
    float FOVdeg,
    float nearPlane,
    float farPlane
)
{
    glm::mat4 view = GetViewMatrix();

    glm::mat4 projection = GetProjectionMatrix(
        FOVdeg,
        nearPlane,
        farPlane
    );

    glm::mat4 uMVP =
        projection * view * modelMatrix;

    glUniformMatrix4fv(
        glGetUniformLocation(shader.ID, uniformName),
        1,
        GL_FALSE,
        glm::value_ptr(uMVP)
    );
}
