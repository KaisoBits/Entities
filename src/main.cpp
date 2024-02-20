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

constexpr float cameraSpeed = 20.0f;
constexpr float mouseSensitivity = 0.1f;

void windowSizeChangeCallback(GLFWwindow* window, int newWidth, int newHeight);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void handleCameraMovement(GLFWwindow* window, float deltaTime);

Camera mainCam(75, static_cast<float>(windowWidth) / windowHeight);

int main()
{
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
		glfwTerminate();
		return -3;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	std::string vertexShader = readFileAsString("shaders/vertexShader.glsl");
	std::string fragmentShader = readFileAsString("shaders/fragmentShader.glsl");

	const ShaderProgram sp = ShaderProgram::Compile(vertexShader, fragmentShader);
	const Model model = ObjParser::LoadFromFile("resources/models/cube.obj");
	Material material1(sp);
	const Texture textureColor = Texture::LoadFromFile("resources/textures/container_color.png");
	const Texture textureSpecular = Texture::LoadFromFile("resources/textures/container_specular.png");
	material1.SetDiffuseMap(&textureColor);
	material1.SetSpecularMap(&textureSpecular);

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

					e->SetRotation(glm::vec3(value / 6.0, value / 8.0f, value / 10.0f));

					value += 2.0f * deltaTime;
				});

			entities.push_back(e);
		}
	}

	constexpr Sun sun = {
		.direction = glm::vec3(1.0, -1, 0),
		.ambient = glm::vec3(0.02f),
		.diffuse = glm::vec3(0.2f),
		.specular = glm::vec3(0.1f)
	};

	PointLight pointLight = {
		.position = glm::vec3(0),
		.diffuse = glm::vec3(40.0f),
		.specular = glm::vec3(40.0f),
		.constant = 1.0f,
		.linear = 5.8f,
		.quadratic = 3.5f,
	};

	const Model groundModel = ObjParser::LoadFromFile("resources/models/ground.obj");
	Material groundMaterial(sp);
	groundMaterial.SetShininess(16);
	const Texture groundTexture = Texture::LoadFromFile("resources/textures/ground_color.jpg");
	const Texture groundSpecTexture = Texture::LoadFromFile("resources/textures/ground_spec.jpg");
	groundMaterial.SetDiffuseMap(&groundTexture);
	groundMaterial.SetSpecularMap(&groundSpecTexture);
	Entity groundEntity(&groundModel, groundMaterial);
	groundEntity.SetPosition(glm::vec3(100, -15, 100));
	groundEntity.SetScale(glm::vec3(20, 1, 20));
	entities.push_back(groundEntity);

	mainCam.SetPosition(glm::vec3(0, 10, 0));
	mainCam.SetRotation(glm::vec2(-136.0f, 21.0f));

	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetWindowSizeCallback(window, windowSizeChangeCallback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glClearColor(160 / 255.0f, 217 / 255.0f, 239 / 255.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);

	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime();
		double deltaTime = fmin(now - lastTime, 0.3f);
		lastTime = now;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		handleCameraMovement(window, static_cast<float>(deltaTime));
		pointLight.position = mainCam.GetPosition();

		for (auto& entity : entities)
		{
			entity.Update(static_cast<float>(deltaTime));
			entity.Draw(mainCam, sun, pointLight);
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