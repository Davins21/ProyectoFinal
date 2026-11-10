#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	this->width = width;
	this->height = height;
	this->Position = position;

	this->Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	this->Up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Velocidad y sensibilidad ajustadas para movimiento fluido
	this->speed = 10.0f;
	this->sensitivity = 0.1f;

	this->yaw = -90.0f;
	this->pitch = 0.0f;
	this->firstMouse = true;
	this->lastX = width / 2.0f;
	this->lastY = height / 2.0f;
}

void Camera::SetupInputs(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, this);
	glfwSetCursorPosCallback(window, Camera::MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Camera::MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	Camera* camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (!camera) return;

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

	camera->ProcessMouseMovement(xoffset, yoffset);
}

void Camera::ProcessInputs(GLFWwindow* window, float deltaTime)
{
	float velocity = speed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Position += velocity * Orientation;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Position -= velocity * Orientation;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Position -= glm::normalize(glm::cross(Orientation, Up)) * velocity;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Position += glm::normalize(glm::cross(Orientation, Up)) * velocity;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		Position += velocity * Up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		Position -= velocity * Up;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (constrainPitch)
	{
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	Orientation = glm::normalize(direction);
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Orientation, Up);
}

glm::mat4 Camera::GetProjectionMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	return glm::perspective(glm::radians(FOVdeg), static_cast<float>(width) / static_cast<float>(height), nearPlane, farPlane);
}

void Camera::UpdateMatrix(Shader& shader, const char* uniformName, glm::mat4 modelMatrix, float FOVdeg, float nearPlane, float farPlane)
{
	glm::mat4 view = GetViewMatrix();
	glm::mat4 projection = GetProjectionMatrix(FOVdeg, nearPlane, farPlane);
	glm::mat4 uMVP = projection * view * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniformName), 1, GL_FALSE, glm::value_ptr(uMVP));
}