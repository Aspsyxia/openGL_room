#include<filesystem>
#include"Model.h"
#include"Skybox.h"

namespace files = std::filesystem;

unsigned int screenWidth = 1600, screenHeight = 800;
bool light0Enabled = true, light1Enabled = true;

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
	Shader shadowMapProgram("shadowMap.vert", "shadowMap.frag");
	Shader shadowCubeMapProgram("shadowCubeMap.vert", "shadowCubeMap.frag", "shadowCubeMap.geom");
	shaderProgram.Activate();
	skyboxProgram.Activate();


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
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
	
	const int n = 30;
	models = new Model[n] {lamp, doorBalconyLeft, doorBalconyRight, fusumaLeft, fusumaRight, doorEntry, tatami, pillow, doorBalconyFrame, writingAccesories, fan, notebook, cupboard, socket, shelf, poster, blanket,chair, pillowChair, fusumaInside, futon, flowerPots, balcony, desk, floorWood, walls, wallsJambs, curtainHolder, curtains, jambsVertical};

	//-----------------------------------------------------------------------------------------------------------------------------

	Model model = lamp;

	// Framebuffer for Shadow Map
	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);

	// Texture for Shadow Map FBO
	unsigned int shadowMapWidth = 1024, shadowMapHeight = 1024;
	unsigned int shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Prevents darkness outside the frustrum
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Matrices needed for the light's perspective
	glm::mat4 orthgonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	glm::mat4 lightView = glm::lookAt(20.0f * lightPos0, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = orthgonalProjection * lightView;

	shadowMapProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowMapProgram.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

	//-----------------------------------------------------------------------------------------------------------------------------

	float farPlane = 100.0f;

	// Framebuffer for Cubemap Shadow Map
	unsigned int pointShadowMapFBO;
	glGenFramebuffers(1, &pointShadowMapFBO);

	// Texture for Cubemap Shadow Map FBO
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Matrices needed for the light's perspective on all faces of the cubemap
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
	glm::mat4 shadowTransforms[] =
	{
	shadowProj * glm::lookAt(lightPos0, lightPos0 + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
	shadowProj * glm::lookAt(lightPos0, lightPos0 + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
	shadowProj * glm::lookAt(lightPos0, lightPos0 + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
	shadowProj * glm::lookAt(lightPos0, lightPos0 + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
	shadowProj * glm::lookAt(lightPos0, lightPos0 + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
	shadowProj * glm::lookAt(lightPos0, lightPos0 + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};
	// Export all matrices to shader
	shadowCubeMapProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[0]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[0]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[1]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[1]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[2]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[2]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[3]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[3]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[4]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[4]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[5]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[5]));
	glUniform3f(glGetUniformLocation(shadowCubeMapProgram.ID, "lightPos"), lightPosDirect.x, lightPosDirect.y, lightPosDirect.z);
	glUniform1f(glGetUniformLocation(shadowCubeMapProgram.ID, "farPlane"), farPlane);


	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.85f, 0.85f, 0.90f, 1.0f);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		// Depth testing needed for Shadow Map
		glEnable(GL_DEPTH_TEST);

		// Preparations for the Shadow Map
		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		// Commented code is for Spotlights and Directional Lights
		//glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		//glClear(GL_DEPTH_BUFFER_BIT);

		//// Draw scene for shadow map
		//for (int i = 0; i < n; i++)
		//{
		//	models[i].Draw(shadowMapProgram, camera);
		//}

		glCullFace(GL_FRONT);
		// Code for Point Lights
		glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		// Draw scene for shadow map
		for (int i = 0; i < n; i++)
		{
			models[i].Draw(shadowCubeMapProgram, camera);
		}
		glCullFace(GL_BACK);

		// Switch back to the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Switch back to the default viewport
		glViewport(0, 0, screenWidth, screenHeight);
		// Bind the custom framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Specify the color of the background
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Enable depth testing since it's disabled when drawing the framebuffer rectangle
		glEnable(GL_DEPTH_TEST);

		shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "farPlane"), farPlane);


		//glActiveTexture(GL_TEXTURE0 + 2);
		//glBindTexture(GL_TEXTURE_2D, shadowMap);
		//glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadowMap"), 2);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadowCubeMap"), 2);

		for (int i = 0; i < n; i++)
		{
			models[i].Draw(shaderProgram, camera);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//model.Draw(shaderProgram, camera);

		glm::vec4 lightCol0, lightCol1;
		if (light0Enabled) lightCol0 = lightColor0;
		else lightCol0 = glm::vec4(0);
		if (light1Enabled) lightCol1 = lightColor1;
		else lightCol1 = glm::vec4(0);

		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor0"), lightCol0.x, lightCol0.y, lightCol0.z, lightCol0.w);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos0"), lightPos0.x, lightPos0.y, lightPos0.z);
		//glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor1"), lightCol1.x, lightCol1.y, lightCol1.z, lightCol1.w);
		//glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos1"), lightPos1.x, lightPos1.y, lightPos1.z);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColorDirect"), lightColorDirect.x, lightColorDirect.y, lightColorDirect.z, lightColorDirect.w);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPosDirect"), lightPosDirect.x, lightPosDirect.y, lightPosDirect.z);
		glUniform1i(glGetUniformLocation(skyboxProgram.ID, "skybox"), 0);

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
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, width, height);
	camera = Camera(width, height, camera.Position);
}