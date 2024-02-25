#include <string>
#include <vector>
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

#include "sun.h"
#include "pointlight.h"
#include "spotlight.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std::chrono_literals;

constexpr int maxLights = 10;

constexpr int windowWidth = 800;
constexpr int windowHeight = 600;

constexpr float cameraSpeed = 20.0f;
constexpr float cameraRunSpeed = 80.0f;
constexpr float mouseSensitivity = 0.1f;

void windowSizeChangeCallback(GLFWwindow* window, int newWidth, int newHeight);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);

void handleCameraMovement(GLFWwindow* window, float deltaTime);

void initImGui(GLFWwindow* window);
void beginFrameImGui(Model& newEntityModel, Material newEntityMaterial);
void endFrameImGui();
void cleanupImGui();
bool imGuiMenuOpen = false;

Camera mainCam(75, static_cast<float>(windowWidth) / windowHeight);

std::vector<Entity> entities;

std::vector<Sun> suns = {
	{
		.direction = glm::vec3(1.0, -1, 0),
		.ambient = glm::vec3(0.02f),
		.diffuse = glm::vec3(0.3f),
		.specular = glm::vec3(0.5f)
	},
};

std::vector<PointLight> pointLights = {
	{
		.position = glm::vec3(0, -9.5f, 0),
		.diffuse = glm::vec3(40.0f),
		.specular = glm::vec3(40.0f),
		.constant = 1.0f,
		.linear = 0.8f,
		.quadratic = 0.01f,
	}
};

std::vector<SpotLight> spotLights = {
	{
		.position = glm::vec3(60, 25, 40),
		.direction = glm::vec3(1, -1, 0),
		.diffuse = glm::vec3(40.0f),
		.specular = glm::vec3(40.0f),
		.constant = 1.0f,
		.linear = 0.8f,
		.quadratic = 0.01f,
		.innerCutoff = glm::cos(glm::radians(15.0f)),
		.outerCutoff = glm::cos(glm::radians(25.0f))
	},
	{
		.position = glm::vec3(30, 25, 70),
		.direction = glm::vec3(1, -1, 0),
		.diffuse = glm::vec3(40.0f),
		.specular = glm::vec3(40.0f),
		.constant = 1.0f,
		.linear = 0.8f,
		.quadratic = 0.01f,
		.innerCutoff = glm::cos(glm::radians(15.0f)),
		.outerCutoff = glm::cos(glm::radians(25.0f))
	}
};

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

	{
		std::string vertexShader = readFileAsString("shaders/vertexShader.glsl");
		std::string fragmentShader = readFileAsString("shaders/fragmentShader.glsl");
		std::string highlightFragmentShader = readFileAsString("shaders/highlightShader.glsl");

		ShaderProgram sp = ShaderProgram::Compile(vertexShader, fragmentShader);
		ShaderProgram hs = ShaderProgram::Compile(vertexShader, highlightFragmentShader);
		Model model = ObjParser::LoadFromFile("resources/models/cube.obj");
		const Texture textureColor = Texture::LoadFromFile("resources/textures/container_color.png");
		const Texture textureSpecular = Texture::LoadFromFile("resources/textures/container_specular.png");
		Material material(&sp, &hs);
		material.SetDiffuseMap(&textureColor);
		material.SetSpecularMap(&textureSpecular);

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				float value = static_cast<float>(i + j) / 2.0f;

				Entity e(&model, material);
				e.SetPosition(glm::vec3(i * 20, 0, j * 20));
				e.SetScale(glm::vec3(5));
				e.SetUpdateFunc(
					[value](Entity* e, float deltaTime) mutable {
						e->SetRotation(glm::vec3(value * 6.0, value * 8.0f, value * 10.0f));
						value += 2.0f * deltaTime;
					});

				entities.push_back(std::move(e));
			}
		}

		const Model groundModel = ObjParser::LoadFromFile("resources/models/ground.obj");
		const Texture groundTexture = Texture::LoadFromFile("resources/textures/ground_color.jpg");
		const Texture groundSpecTexture = Texture::LoadFromFile("resources/textures/ground_spec.jpg");
		Material groundMaterial(&sp, &hs);
		groundMaterial.SetShininess(16);
		groundMaterial.SetDiffuseMap(&groundTexture);
		groundMaterial.SetSpecularMap(&groundSpecTexture);
		Entity groundEntity(&groundModel, groundMaterial);
		groundEntity.SetPosition(glm::vec3(100, -15, 100));
		groundEntity.SetScale(glm::vec3(20, 1, 20));
		entities.push_back(std::move(groundEntity));

		mainCam.SetPosition(glm::vec3(0, 10, 0));
		mainCam.SetRotation(glm::vec2(-136.0f, 21.0f));

		glViewport(0, 0, windowWidth, windowHeight);
		glfwSetWindowSizeCallback(window, windowSizeChangeCallback);
		glfwSetCursorPosCallback(window, mouseCallback);
		glfwSetKeyCallback(window, handleKey);
		initImGui(window);

		glClearColor(160 / 7.0f / 255.0f, 217 / 7.0f / 255.0f, 239 / 7.0f / 255.0f, 1.0f);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);

		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		double lastTime = glfwGetTime();
		while (!glfwWindowShouldClose(window))
		{
			double now = glfwGetTime();
			double deltaTime = fmin(now - lastTime, 0.3f);
			lastTime = now;

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glfwPollEvents();

			beginFrameImGui(model, material);

			handleCameraMovement(window, static_cast<float>(deltaTime));

			for (auto& entity : entities)
			{
				entity.Update(static_cast<float>(deltaTime));
				entity.Draw(mainCam, suns, pointLights, spotLights);

				// Clear menu highlight
				if (imGuiMenuOpen && entity.GetIsHighlighted())
					entity.SetIsHighlighted(false);
			}

			endFrameImGui();

			glfwSwapBuffers(window);
		}
	}

	cleanupImGui();
	glfwTerminate();

	return 0;
}

void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// The ~ key
	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
	{
		imGuiMenuOpen = !imGuiMenuOpen;
		if (imGuiMenuOpen)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void handleCameraMovement(GLFWwindow* window, float deltaTime)
{
	if (imGuiMenuOpen)
		return;

	constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float speed = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ?
		cameraRunSpeed : cameraSpeed;

	glm::vec3 position = mainCam.GetPosition();

	const glm::vec3 forwardVectorJustYaw = mainCam.ForwardJustYaw();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += forwardVectorJustYaw * deltaTime * speed;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= forwardVectorJustYaw * deltaTime * speed;

	const glm::vec3 right = glm::cross(forwardVectorJustYaw, up);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += right * deltaTime * speed;
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= right * deltaTime * speed;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += up * deltaTime * speed;
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		position -= up * deltaTime * speed;

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

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (!imGuiMenuOpen)
	{
		glm::vec2 camRotation = mainCam.GetRotation();

		camRotation.x += static_cast<float>(xpos - lastX) * mouseSensitivity;
		camRotation.y = fmin(fmax(camRotation.y + static_cast<float>(ypos - lastY) * mouseSensitivity, -89.0f), 89.0f);

		mainCam.SetRotation(camRotation);
	}

	lastX = xpos;
	lastY = ypos;
}

void initImGui(GLFWwindow* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

int selectedEntity = 0;
int selectedSun = 0;
int selectedPointLight = 0;
int selectedSpotLight = 0;
void beginFrameImGui(Model& newEntityModel, Material newEntityMaterial)
{
	if (!imGuiMenuOpen)
		return;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug menu");

	if (ImGui::TreeNode("Entities"))
	{
		if (entities.size() > 0)
		{
			ImGui::SliderInt("Selected##entity", &selectedEntity, 0, entities.size() - 1);
			Entity& entity = entities[selectedEntity];

			ImGui::Spacing();

			glm::vec3 entityPosition = entity.GetPosition();
			glm::vec3 entityRotation = entity.GetRotation();
			glm::vec3 entityScale = entity.GetScale();
			bool entityShouldUpdate = entity.GetShouldUpdate();
			ImGui::DragFloat3("Position##entity", &entityPosition[0], 0.05f);
			ImGui::DragFloat3("Rotation##entity", &entityRotation[0], 0.05f);
			ImGui::DragFloat3("Scale##entity", &entityScale[0], 0.05f);
			ImGui::Checkbox("Update##entity", &entityShouldUpdate);
			entity.SetPosition(entityPosition);
			entity.SetRotation(entityRotation);
			entity.SetScale(entityScale);
			entity.SetShouldUpdate(entityShouldUpdate);

			if (ImGui::Button("Move to camera##entity"))
			{
				entity.SetPosition(mainCam.GetPosition());
			}
			ImGui::SameLine();
		}
		if (ImGui::Button("Add##entity"))
		{
			float value = 0;
			Entity e(&newEntityModel, newEntityMaterial);
			e.SetPosition(mainCam.GetPosition());
			e.SetUpdateFunc(
				[value](Entity* e, float deltaTime) mutable {
					e->SetRotation(glm::vec3(value * 6.0, value * 8.0f, value * 10.0f));
					value += 2.0f * deltaTime;
				});

			entities.push_back(std::move(e));

			selectedEntity = entities.size() - 1;
		}
		ImGui::SameLine();
		if (entities.size() > 0 && ImGui::Button("Delete##entity"))
		{
			entities.erase(entities.begin() + selectedEntity);
			if (selectedEntity == entities.size())
				selectedEntity--;
		}
		ImGui::TreePop();
		ImGui::Spacing();

		if (selectedEntity >= 0)
			entities[selectedEntity].SetIsHighlighted(true);
	}

	if (ImGui::TreeNode("Sun controls"))
	{
		if (suns.size() > 0)
		{
			ImGui::SliderInt("Selected##sun", &selectedSun, 0, suns.size() - 1);
			Sun& sun = suns[selectedSun];

			ImGui::DragFloat3("Ambient##sun", &sun.ambient[0], 0.05f);
			ImGui::DragFloat3("Diffuse##sun", &sun.diffuse[0], 0.05f);
			ImGui::DragFloat3("Specular##sun", &sun.specular[0], 0.05f);

			ImGui::Spacing();

			ImGui::DragFloat3("Direction##sun", &sun.direction[0], 0.05f);
			if (ImGui::Button("Align with camera##sun"))
				sun.direction = mainCam.Forward();
			ImGui::SameLine();
		}
		if (suns.size() < maxLights && ImGui::Button("Add##sun"))
		{
			suns.emplace_back(Sun
				{
					.direction = mainCam.Forward(),
					.ambient = glm::vec3(0.02f),
					.diffuse = glm::vec3(0.3f),
					.specular = glm::vec3(0.5f)
				});

			selectedSun = suns.size() - 1;
		}
		ImGui::SameLine();
		if (suns.size() > 0 && ImGui::Button("Delete##sun"))
		{
			suns.erase(suns.begin() + selectedSun);
			if (selectedSun == suns.size())
				selectedSun--;
		}

		ImGui::TreePop();
		ImGui::Spacing();
	}

	if (ImGui::TreeNode("Pointlight controls"))
	{
		if (pointLights.size() > 0)
		{
			ImGui::SliderInt("Selected##pointlight", &selectedPointLight, 0, pointLights.size() - 1);
			PointLight& pointLight = pointLights[selectedPointLight];

			ImGui::SliderFloat("Constant##pointlight", &pointLight.constant, 0.0f, 10.0f);
			ImGui::SliderFloat("Linear##pointlight", &pointLight.linear, 0.0f, 1.0f);
			ImGui::SliderFloat("Quadratic##pointlight", &pointLight.quadratic, 0.0f, 0.1f, "%.5f");

			ImGui::Spacing();

			ImGui::DragFloat3("Diffuse##pointlight", &pointLight.diffuse[0], 0.05f);
			ImGui::DragFloat3("Specular##pointlight", &pointLight.specular[0], 0.05f);

			ImGui::Spacing();

			ImGui::DragFloat3("Position##pointlight", &pointLight.position[0], 0.05f);
			if (ImGui::Button("Move to camera##pointlight"))
				pointLight.position = mainCam.GetPosition();
			ImGui::SameLine();
		}
		if (pointLights.size() < maxLights && ImGui::Button("Add##pointlight"))
		{
			pointLights.emplace_back(PointLight
				{
					.position = mainCam.GetPosition(),
					.diffuse = glm::vec3(40.0f),
					.specular = glm::vec3(40.0f),
					.constant = 1.0f,
					.linear = 0.8f,
					.quadratic = 0.01f,
				});

			selectedPointLight = pointLights.size() - 1;
		}
		ImGui::SameLine();
		if (pointLights.size() > 0 && ImGui::Button("Delete##pointlight"))
		{
			pointLights.erase(pointLights.begin() + selectedPointLight);
			if (selectedPointLight == pointLights.size())
				selectedPointLight--;
		}

		ImGui::TreePop();
		ImGui::Spacing();
	}

	if (ImGui::TreeNode("Spotlight controls"))
	{
		if (spotLights.size() > 0)
		{
			ImGui::SliderInt("Selected##spotlight", &selectedSpotLight, 0, spotLights.size() - 1);
			SpotLight& spotLight = spotLights[selectedSpotLight];

			ImGui::SliderFloat("Constant##spotlight", &spotLight.constant, 0.0f, 10.0f);
			ImGui::SliderFloat("Linear##spotlight", &spotLight.linear, 0.0f, 1.0f);
			ImGui::SliderFloat("Quadratic##spotlight", &spotLight.quadratic, 0.0f, 0.1f, "%.5f");

			ImGui::Spacing();

			ImGui::SliderFloat("Inner cutoff##spotlight", &spotLight.innerCutoff, spotLight.outerCutoff, 1.0f);
			ImGui::SliderFloat("Outer cutoff##spotlight", &spotLight.outerCutoff, 0.0f, spotLight.innerCutoff);

			ImGui::Spacing();

			ImGui::DragFloat3("Diffuse##spotlight", &spotLight.diffuse[0], 0.05f);
			ImGui::DragFloat3("Specular##spotlight", &spotLight.specular[0], 0.05f);

			ImGui::Spacing();

			ImGui::DragFloat3("Position##spotlight", &spotLight.position[0], 0.05f);
			ImGui::DragFloat3("Direction##spotlight", &spotLight.direction[0], 0.05f);
			if (ImGui::Button("Align with camera##spotlight"))
			{
				spotLight.position = mainCam.GetPosition();
				spotLight.direction = mainCam.Forward();
			}
			ImGui::SameLine();
		}
		if (spotLights.size() < maxLights && ImGui::Button("Add##spotlight"))
		{
			spotLights.emplace_back(SpotLight
				{
					.position = mainCam.GetPosition(),
					.direction = mainCam.Forward(),
					.diffuse = glm::vec3(40.0f),
					.specular = glm::vec3(40.0f),
					.constant = 1.0f,
					.linear = 0.8f,
					.quadratic = 0.01f,
					.innerCutoff = glm::cos(glm::radians(15.0f)),
					.outerCutoff = glm::cos(glm::radians(25.0f))
				});
			selectedSpotLight = spotLights.size() - 1;
		}
		ImGui::SameLine();
		if (spotLights.size() > 0 && ImGui::Button("Delete##spotlight"))
		{
			spotLights.erase(spotLights.begin() + selectedSpotLight);
			if (selectedSpotLight == spotLights.size())
				selectedSpotLight--;
		}
		ImGui::TreePop();
		ImGui::Spacing();
	}

	ImGui::End();
}

void endFrameImGui()
{
	if (!imGuiMenuOpen)
		return;

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cleanupImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}