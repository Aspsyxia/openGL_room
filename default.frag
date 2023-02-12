#version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;
in vec4 fragPosLight;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D shadowMap;
uniform samplerCube shadowCubeMap;
uniform float farPlane;

uniform vec4 lightColor0;
uniform vec3 lightPos0;
uniform vec4 lightColor1;
uniform vec3 lightPos1;
uniform vec4 lightColorDirect;
uniform vec3 lightPosDirect;
uniform vec3 camPos;

float near = 0.1f;
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

	// Shadow value
	float shadow = 0.0f;
	vec3 fragToLight = crntPos - lightPos;
	float currentDepth = length(fragToLight);
	float bias = max(0.5f * (1.0f - dot(normal, lightDirection)), 0.0005f); 

	// Not really a radius, more like half the width of a square
	int sampleRadius = 2;
	float offset = 0.09f;
	for(int z = -sampleRadius; z <= sampleRadius; z++)
	{
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowCubeMap, fragToLight + vec3(x, y, z) * offset).r;
				// Remember that we divided by the farPlane?
				// Also notice how the currentDepth is not in the range [0, 1]
				closestDepth *= farPlane;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
	}
	// Average shadow
	shadow /= pow((sampleRadius * 2 + 1), 3);

	return (texture(diffuse0, texCoord) * (diffuse * (1.0f - shadow) * inten + ambient) + texture(specular0, texCoord).r * specular * (1.0f - shadow) * inten) * lightColor;
}

vec4 directLight(vec4 lightColor)
{
	// ambient lighting
	float ambient = 0.50f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPosDirect);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);

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

void main()
{
	float depth = logisticDepth(gl_FragCoord.z);
	vec4 pointLight0 = pointLight(lightColor0, lightPos0, 3.0, 0.09);
	vec4 pointLightDirect = pointLight(lightColorDirect, lightPosDirect, 1.5, 0.5);
	FragColor = pointLight0 + pointLightDirect; //+ pointLightDirect;
}