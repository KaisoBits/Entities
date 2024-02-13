#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderprogram.h"
#include "model.h"
#include "objparser.h"
#include "fileutils.h"
#include "material.h"
#include "texture.h"
#include "entity.h"

using namespace std::chrono_literals;

const int windowWidth = 800;
const int windowHeight = 600;

const float cameraSpeed = 60.0f;
const float mouseSensitivity = 1.0f;

void windowSizeChangeCallback(GLFWwindow* window, int newWidth, int newHeight);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void handleCameraMovement(GLFWwindow* window, float deltaTime);
void updateTimeUniform();

Camera mainCam(75, (float)windowWidth / windowHeight);

int main()
{
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Hello again", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return -3;
	}

	std::string vertexShader = readFileAsString("shaders/vertexShader.glsl");
	std::string fragmentShader = readFileAsString("shaders/fragmentShader.glsl");

	const ShaderProgram sp = ShaderProgram::Compile(vertexShader, fragmentShader);
	const Model model = ObjParser::LoadFromFile("models/cat2.obj");
	Material material1(sp);
	const Texture textureCat = Texture::LoadFromFile("textures/cat2.jpg");
	material1.AddTexture(textureCat);

	std::vector<Entity> entities;

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			float value = (i + j) / 2.0f;

			Entity e(&model, material1);
			e.SetPosition(glm::vec3(i * 20, 0, j * 20));
			e.SetUpdateFunc([value](Entity* e, float deltaTime) mutable {
				const float animationHeight = 10.0f;

				glm::vec3 position = e->GetPosition();
				position.y = sin(value) * animationHeight;
				e->SetPosition(position);

				e->SetRotation(glm::vec3(0, value / 3.0f, 0));

				value += 2.0f * deltaTime;
				});

			entities.push_back(e);
		}
	}

	const Model groundModel = ObjParser::LoadFromFile("models/ground.obj");
	Material groundMaterial(sp);
	const Texture groundTexture = Texture::LoadFromFile("textures/ground_color.jpg");
	groundMaterial.AddTexture(groundTexture);
	Entity groundEntity(&groundModel, groundMaterial);
	groundEntity.SetScale(glm::vec3(10, 10, 10));
	entities.push_back(groundEntity);

	mainCam.SetPosition(glm::vec3(-36.0f, 20.0f, 0.5f));
	mainCam.SetRotation(glm::vec2(-136.0f, 21.0f));

	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetWindowSizeCallback(window, windowSizeChangeCallback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glClearColor(60.0f / 255, 60.0f / 255, 60.0f / 255, 1.0f);
	glEnable(GL_DEPTH_TEST);

	float lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		float now = glfwGetTime();
		float deltaTime = fmin(now - lastTime, 0.3f);
		lastTime = now;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		handleCameraMovement(window, deltaTime);

		updateTimeUniform();

		for (auto& entity : entities)
		{
			entity.Update(deltaTime);
			entity.Draw(mainCam);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		std::this_thread::yield();
	}

	glfwTerminate();

	return 0;
}

void handleCameraMovement(GLFWwindow* window, float deltaTime)
{
	const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 position = mainCam.GetPosition();

	glm::vec3 forwardVectorJustYaw = mainCam.ForwardJustYaw();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += forwardVectorJustYaw * deltaTime * cameraSpeed;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= forwardVectorJustYaw * deltaTime * cameraSpeed;

	glm::vec3 right = glm::cross(forwardVectorJustYaw, up);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += right * deltaTime * cameraSpeed;
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= right * deltaTime * cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += up * deltaTime * cameraSpeed;
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		position -= up * deltaTime * cameraSpeed;

	mainCam.SetPosition(position);

	std::this_thread::sleep_for(1ms);
}

void windowSizeChangeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	if (newWidth == 0 || newHeight == 0) // ignore minimizing
		return;

	glViewport(0, 0, newWidth, newHeight);
	mainCam.SetAspectRatio((float)newWidth / newHeight);
}

double lastX = -1;
double lastY = -1;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (lastX >= 0)
	{
		glm::vec2 camRotation = mainCam.GetRotation();

		camRotation.x += (xpos - lastX) * mouseSensitivity;
		camRotation.y = fmin(fmax(camRotation.y + (ypos - lastY) * mouseSensitivity, -89.0f), 89.0f);

		mainCam.SetRotation(camRotation);
	}

	lastX = xpos;
	lastY = ypos;
}

void updateTimeUniform()
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);

	int location = glGetUniformLocation(id, "time");
	if (location >= 0)
	{
		float time = (float)glfwGetTime();

		glUniform1f(location, time);
	}
}