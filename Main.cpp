#include"Model.h"
#include"Skybox.h"

namespace files = std::filesystem;

unsigned int screenWidth = 1600, screenHeight = 800;
bool light0Enabled = true, light1Enabled = true, fogEnabled = false;

Camera camera(screenWidth, screenHeight, glm::vec3(0.5f, 1.0f, 0.5f));
Model* models;

glm::vec3 lightPos0 = glm::vec3(-1.3f, 1.0f, -1.1f), lightPos1 = glm::vec3(1.0f, 2.0f, 0.0f), lightPosDirect = glm::vec3(0.0f, 3.0f, -3.0f);
glm::vec4 lightColor0 = glm::vec4(0.9f, 0.8f, 0.5f, 1.0f), lightColor1 = glm::vec4(1), lightColorDirect = glm::vec4(0.9f, 0.6f, 0.35f, 1.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Room", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, screenWidth, screenHeight);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxProgram("skybox.vert", "skybox.frag");
	shaderProgram.Activate();
	skyboxProgram.Activate();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	Skybox skyboxNew;

	Model tatami("models/tatami/tatami.gltf");
	Model doorBalconyFrame("models/doorBalconySeperate/doorBalconyFrame1.gltf");
	Model doorBalconyLeft("models/doorBalconySeperate/doorBalconyLeft.gltf");
	Model doorBalconyRight("models/doorBalconySeperate/doorBalconyRight1.gltf");
	Model fusumaLeft("models/fusumaSeperate/fusumaLeft.gltf");
	Model fusumaRight("models/fusumaSeperate/fusumaRight.gltf");
	Model floorWood("models/floorWood/floorWood.gltf");
	Model wallsJambs("models/wallsJambs/wallsJambs.gltf");
	Model jambsVertical("models/jambsVertical/jambsVertical.gltf");
	Model curtainHolder("models/curtainHolder/curtainHolder.gltf");
	Model walls("models/walls/walls.gltf");
	Model curtains("models/curtains/curtains.gltf");
	Model desk("models/desk/desk.gltf");
	Model futon("models/futon/futon.gltf");
	Model balcony("models/balcony/balcony.gltf");
	Model flowerPots("models/flowerPots/flowerPots.gltf");
	Model fusumaInside("models/fusumaInside/fusumaInside.gltf");
	Model blanket("models/blanket/blanket.gltf");
	Model pillow("models/pillow/pillow.gltf");
	Model pillowChair("models/pillowChair/pillowChair.gltf");
	Model chair("models/chair/chair.gltf");
	Model poster("models/poster_1/poster.gltf");
	Model shelf("models/shelf/shelf.gltf");
	Model lamp("models/lamp/lamp.gltf");
	Model socket("models/socket/socket.gltf");
	Model doorEntry("models/doorEntry/doorEntry.gltf");
	Model cupboard("models/cupboard/cupboard.gltf");
	Model notebook("models/notebook/notebook.gltf");
	Model fan("models/fan/fan.gltf");
	Model writingAccesories("models/writingAccesories/writingAccesories.gltf");
	Model books("models/books/books.gltf");
	Model cigarettes("models/cigarettes/cigarettes.gltf");
	Model corcBoard("models/corcBoard/corcBoard.gltf");
	Model kayako("models/kayako/kayako.gltf");
	Model lampCeiling("models/lampCeiling/lampCeiling.gltf");
	Model flowers("models/flowers/flowers.gltf");
	Model flowersGrass("models/flowersGrass/flowersGrass.gltf");
	Model picture("models/picture/picture.gltf");
	Model rubbishBean("models/rubbishBean/rubbishBean.gltf");
	Model ac("models/ac/ac.gltf");

	const int n = 40;
	models = new Model[n] {doorBalconyLeft, doorBalconyRight, fusumaLeft, fusumaRight, doorEntry, ac, rubbishBean, picture, flowersGrass, flowers, lampCeiling, kayako, cigarettes, corcBoard, tatami, books, pillow, doorBalconyFrame, writingAccesories, fan, notebook, cupboard, socket, lamp, shelf, poster, blanket,chair, pillowChair, fusumaInside, futon, flowerPots, balcony, desk, floorWood, walls, wallsJambs, curtainHolder, curtains, jambsVertical};

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.85f, 0.85f, 0.90f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		for (int i = 0; i < n; i++)
		{
			models[i].Draw(shaderProgram, camera);
		}

		glm::vec4 lightCol0, lightCol1;
		glm::vec3 fogColor;
		if (light0Enabled) lightCol0 = lightColor0;
		else lightCol0 = glm::vec4(0);
		if (light1Enabled) lightCol1 = lightColor1;
		else lightCol1 = glm::vec4(0);
		if (fogEnabled) fogColor = glm::vec3(0.9f, 0.6f, 0.35f);
		else fogColor = glm::vec3(0);

		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor0"), lightCol0.x, lightCol0.y, lightCol0.z, lightCol0.w);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos0"), lightPos0.x, lightPos0.y, lightPos0.z);
		//glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor1"), lightCol1.x, lightCol1.y, lightCol1.z, lightCol1.w);
		//glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos1"), lightPos1.x, lightPos1.y, lightPos1.z);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColorDirect"), lightColorDirect.x, lightColorDirect.y, lightColorDirect.z, lightColorDirect.w);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPosDirect"), lightPosDirect.x, lightPosDirect.y, lightPosDirect.z);
		glUniform1i(glGetUniformLocation(skyboxProgram.ID, "skybox"), 0);

		glUniform3f(glGetUniformLocation(shaderProgram.ID, "fogColor"), fogColor.x, fogColor.y, fogColor.z);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "density"), 0.2f);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "gradient"), 0.9f);


		glDisable(GL_CULL_FACE);
		skyboxNew.Draw(skyboxProgram, camera, screenWidth, screenHeight);
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


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		glm::vec3 movement = glm::vec3(0.75f, 0, 0);
		models[0].interacted = !models[0].interacted;
		if (models[0].interacted) models[0].Move(movement);
		else models[0].Move(-movement);
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		glm::vec3 movement = glm::vec3(-0.75f, 0, 0);
		models[1].interacted = !models[1].interacted;
		if (models[1].interacted) models[1].Move(movement);
		else models[1].Move(-movement);
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		glm::vec3 movement = glm::vec3(0, 0, 0.65f);
		models[2].interacted = !models[2].interacted;
		if (models[2].interacted) models[2].Move(movement);
		else models[2].Move(-movement);
	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		glm::vec3 movement = glm::vec3(0, 0, -0.65f);
		models[3].interacted = !models[3].interacted;
		if (models[3].interacted) models[3].Move(movement);
		else models[3].Move(-movement);
	}
	else if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		glm::vec3 movement = glm::vec3(0.75f, 0, 0);
		models[4].interacted = !models[4].interacted;
		if (models[4].interacted) models[4].Move(movement);
		else models[4].Move(-movement);
	}
	else if (key == GLFW_KEY_9 && action == GLFW_PRESS) light0Enabled = !light0Enabled;
	else if (key == GLFW_KEY_0 && action == GLFW_PRESS) light1Enabled = !light1Enabled;
	else if (key == GLFW_KEY_8 && action == GLFW_PRESS) fogEnabled = !fogEnabled;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, width, height);
	camera = Camera(width, height, camera.Position);
}