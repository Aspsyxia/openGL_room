#include<filesystem>
#include"Model.h"
#include"Skybox.h"

namespace files = std::filesystem;

const unsigned int width = 1600;
const unsigned int height = 800;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Room", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(0, 0, width, height);

	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxProgram("skybox.vert", "skybox.frag");

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 2.0f, 0.0f);
	glm::mat4 lightModel = glm::mat4(3.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();

	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	skyboxProgram.Activate();

	glUniform1i(glGetUniformLocation(skyboxProgram.ID, "skybox"), 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	Camera camera(width, height, glm::vec3(0.5f, 1.0f, 0.5f));
	Skybox skyboxNew;
	
	const int n = 9;

	Model tatami("models/tatami/tatami.gltf");
	Model doorBalcony("models/doorBalcony/doorBalcony.gltf");
	Model doorFusuma("models/doorFusuma/doorFusuma.gltf");
	Model floorWood("models/floorWood/floorWood.gltf");
	Model wallsJambs("models/wallsJambs/wallsJambs.gltf");
	Model jambsVertical("models/jambsVertical/jambsVertical.gltf");
	Model curtainHolder("models/curtainHolder/curtainHolder.gltf");
	Model walls("models/walls/walls.gltf");
	Model curtains("models/curtains/curtains.gltf");

	Model models[n] = { tatami, doorBalcony, doorFusuma, floorWood, walls, wallsJambs, curtainHolder, curtains, jambsVertical };

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.85f, 0.85f, 0.90f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		for (int i = 0; i <= n; i++)
		{
			models[i].Draw(shaderProgram, camera);
		}

		glDisable(GL_CULL_FACE);

		skyboxNew.Draw(skyboxProgram, camera, width, height);

		glEnable(GL_CULL_FACE);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	skyboxProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}