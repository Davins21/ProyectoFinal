#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderClass.h"

class Camera {
public:
	// Atributos de transformación
	glm::vec3 Position;
	glm::vec3 Orientation;
	glm::vec3 Up;

	// Dimensiones de la ventana
	int width;
	int height;

	// Configuración de movimiento y sensibilidad
	float speed;
	float sensitivity;

	// Variables para el control de rotación del mouse
	float yaw;
	float pitch;
	bool firstMouse;
	float lastX;
	float lastY;

	// Constructor
	Camera(int width, int height, glm::vec3 position);

	// Registra callbacks e inputs de GLFW
	void SetupInputs(GLFWwindow* window);

	// Callback estático para el movimiento del mouse
	static void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);

	// Procesar entradas de teclado (WASD, Espacio, Shift)
	void ProcessInputs(GLFWwindow* window, float deltaTime);

	// Procesar rotación de visión
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

	// Métodos para generar matrices
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix(float FOVdeg, float nearPlane, float farPlane);

	// Actualizar y enviar la matriz MVP unificada al Shader
	void UpdateMatrix(Shader& shader, const char* uniformName, glm::mat4 modelMatrix, float FOVdeg = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);
};

#endif