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

constexpr int windowWidth = 800;
constexpr int windowHeight = 600;

constexpr float cameraSpeed = 60.0f;
constexpr float mouseSensitivity = 0.2f;

void windowSizeChangeCallback(GLFWwindow* window, int newWidth, int newHeight);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void handleCameraMovement(GLFWwindow* window, float deltaTime);

Camera mainCam(75, static_cast<float>(windowWidth) / windowHeight);

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

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		return -3;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	std::string vertexShader = readFileAsString("shaders/vertexShader.glsl");
	std::string fragmentShader = readFileAsString("shaders/fragmentShader.glsl");

	const ShaderProgram sp = ShaderProgram::Compile(vertexShader, fragmentShader);
	const Model model = ObjParser::LoadFromFile("models/monkey.obj");
	Material material1(sp);
	const Texture textureCat = Texture::LoadFromFile("textures/ground_color.jpg");
	material1.AddTexture(textureCat);

	std::vector<Entity> entities;

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			float value = static_cast<float>(i + j) / 2.0f;

			Entity e(&model, material1);
			e.SetPosition(glm::vec3(i * 20, 0, j * 20));
			e.SetScale(glm::vec3(5));
			e.SetUpdateFunc(
				[value](Entity* e, float deltaTime) mutable {
					constexpr float animationHeight = 10.0f;

					glm::vec3 position = e->GetPosition();
					position.y = sin(value) * animationHeight;
					// e->SetPosition(position);

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
	groundEntity.SetPosition(glm::vec3(100, -15, 100));
	groundEntity.SetScale(glm::vec3(20, 1, 20));
	entities.push_back(groundEntity);

	mainCam.SetPosition(glm::vec3(0, 10, 0));
	mainCam.SetRotation(glm::vec2(-136.0f, 21.0f));

	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetWindowSizeCallback(window, windowSizeChangeCallback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glClearColor(60.0f / 255, 60.0f / 255, 60.0f / 255, 1.0f);
	glEnable(GL_DEPTH_TEST);

	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime();
		double deltaTime = fmin(now - lastTime, 0.3f);
		lastTime = now;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		handleCameraMovement(window, static_cast<float>(deltaTime));

		for (auto& entity : entities)
		{
			entity.Update(static_cast<float>(deltaTime));
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
	constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 position = mainCam.GetPosition();

	const glm::vec3 forwardVectorJustYaw = mainCam.ForwardJustYaw();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += forwardVectorJustYaw * deltaTime * cameraSpeed;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= forwardVectorJustYaw * deltaTime * cameraSpeed;

	const glm::vec3 right = glm::cross(forwardVectorJustYaw, up);
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
	mainCam.SetAspectRatio(static_cast<float>(newWidth) / static_cast<float>(newHeight));
}

double lastX = -1;
double lastY = -1;
bool firstInput = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstInput)
	{
		firstInput = false;
		return;
	}

	glm::vec2 camRotation = mainCam.GetRotation();

	camRotation.x += static_cast<float>(xpos - lastX) * mouseSensitivity;
	camRotation.y = fmin(fmax(camRotation.y + static_cast<float>(ypos - lastY) * mouseSensitivity, -89.0f), 89.0f);

	mainCam.SetRotation(camRotation);

	lastX = xpos;
	lastY = ypos;
}