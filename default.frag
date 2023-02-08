#version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;
uniform sampler2D displacement0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

float near = 0.1f;
float far = 100.0f;


vec4 pointLight()
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.30f;

	vec3 viewDirection = normalize(camPos - crntPos);
	
	// Variables that control parallax occlusion mapping quality
	float heightScale = 0.05f;
	const float minLayers = 8.0f;
    const float maxLayers = 64.0f;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0f, 0.0f, 1.0f), viewDirection)));
	float layerDepth = 1.0f / numLayers;
	float currentLayerDepth = 0.0f;
	
	// Remove the z division if you want less aberated results
	vec2 S = viewDirection.xy / viewDirection.z * heightScale; 
    vec2 deltaUVs = S / numLayers;
	
	vec2 UVs = texCoord;
	float currentDepthMapValue = 1.0f - texture(displacement0, UVs).r;
	
	// Loop till the point on the heightmap is "hit"
	while(currentLayerDepth < currentDepthMapValue)
    {
        UVs -= deltaUVs;
        currentDepthMapValue = 1.0f - texture(displacement0, UVs).r;
        currentLayerDepth += layerDepth;
    }

	// Apply Occlusion (interpolation with prev value)
	vec2 prevTexCoords = UVs + deltaUVs;
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = 1.0f - texture(displacement0, prevTexCoords).r - currentLayerDepth + layerDepth;
	float weight = afterDepth / (afterDepth - beforeDepth);
	UVs = prevTexCoords * weight + UVs * (1.0f - weight);

	// diffuse lighting
	vec3 normal = normalize(texture(normal0, UVs).xyz * 2.0f - 1.0f);
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

	return (texture(diffuse0, UVs) * (diffuse * inten + ambient) + texture(specular0, UVs).r * specular * inten) * lightColor;
}

vec4 direcLight()
{
	// ambient lighting
	float ambient = 0.50f;

	// diffuse lighting
	vec3 normal = normalize(texture(normal0, texCoord).xyz * 2.0f - 1.0f);
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

void main()
{
	float depth = logisticDepth(gl_FragCoord.z);
	FragColor = pointLight() * (1.0f - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);
}