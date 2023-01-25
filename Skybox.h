#ifndef SKYBOX_CLASS_H
#define SKYBOX_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"
#include"Mesh.h"

class Skybox
{
public:

	Skybox();

	void Draw(Shader& shader, Camera& camera, float width, float height);

};

#endif
