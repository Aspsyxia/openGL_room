#include<filesystem>
#include"Model.h"

namespace fs = std::filesystem;

const unsigned int width = 1600;
const unsigned int height = 800;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);

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

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 2.0f, 0.0f);
	glm::mat4 lightModel = glm::mat4(3.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();

	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));
	
	const int n = 7;

	Model tatami("models/tatami/tatami.gltf");
	Model doorBalcony("models/doorBalcony/doorBalcony.gltf");
	Model floorWood("models/floorWood/floorWood.gltf");
	Model wallsJambs("models/wallsJambs/wallsJambs.gltf");
	Model curtainHolder("models/curtainHolder/curtainHolder.gltf");
	Model walls("models/walls/walls.gltf");
	Model curtains("models/curtains/curtains.gltf");

	Model models[n] = { tatami, doorBalcony, floorWood, walls, wallsJambs, curtainHolder, curtains };

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(.0f, .0f, 0.0f, .0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		for (int i = 0; i <= n; i++)
		{
			models[i].Draw(shaderProgram, camera);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}