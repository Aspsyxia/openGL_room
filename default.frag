#version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;
//uniform sampler2D texture1;

uniform vec4 lightColor0;
uniform vec3 lightPos0;
uniform vec4 lightColor1;
uniform vec3 lightPos1;
uniform vec4 lightColorDirect;
uniform vec3 lightPosDirect;
uniform vec3 camPos;

uniform vec3 fogColor;
uniform float density;
uniform float gradient;

float near = 0.2f;
float far = 100.0f;

vec4 pointLight(vec4 lightColor, vec3 lightPos, float a, float b)
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float inten = 5.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.30f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if(diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);

		vec3 halfwayVector = normalize(viewDirection + lightDirection);

		float specAmount = pow(max(dot(normal, halfwayVector), 0.0f), 16);
		float specular = specAmount * specularLight;
	};

	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}

vec4 directLight(vec4 lightColor)
{
	// ambient lighting
	float ambient = 0.50f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);

	vec3 halfwayVector = normalize(viewDirection + lightDirection);


	float specAmount = pow(max(dot(normal, halfwayVector), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

float depthToLinear(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth)
{
	float steepness = 0.5f;
	float offset = 5.0f;
	float zVal = depthToLinear(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}

float calculateFog(float zCoord)
{
    float z = abs(zCoord);
    return exp(-density * z * gradient);
}

void main()
{
	float depth = logisticDepth(gl_FragCoord.z);
	vec4 pointLight0 = pointLight(lightColor0, lightPos0, 3.0, 0.9);
	vec4 pointLight1 = pointLight(lightColor1, lightPos1, 3.0, 0.9);
	vec4 pointLightDirect = pointLight(lightColorDirect, lightPosDirect, 1.5, 0.5);
	float fogFactor = calculateFog(gl_FragCoord.z);
	FragColor = pointLight0 + pointLight1 + mix(pointLightDirect*(1.0f - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f), vec4(fogColor, 1.0), fogFactor); //(1.0f - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);
}

//texture(texture1, texCoord)