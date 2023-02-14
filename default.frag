#version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;
uniform sampler2D lightBuffer;

uniform vec3 lightColor0;
uniform vec3 lightPos0;
uniform vec4 lightColor1;
uniform vec3 lightPos1;
uniform vec4 lightColorDirect;
uniform vec3 lightPosDirect;
uniform vec3 camPos;

uniform float dotFactor;
uniform float roughnessFactor;

uniform vec3 fogColor;
uniform float density;
uniform float gradient;

float near = 0.2f;
float far = 100.0f;

vec4 pointLight(vec3 lightColor, vec3 lightPos, float a, float b, float dotFactor, float roughnessFactor)
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float inten = 3.0f / (a * dist * dist + b * dist + 1.0f);

	vec3 albedo = texture(diffuse0, texCoord).rgb;
    vec3 normal = texture(normal0, texCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);
    float roughness = texture(specular0, texCoord).r;

	// ambient lighting
	vec3 ambient = lightColor * albedo * 0.5f;

	// diffuse lighting
	vec3 lightDirection = normalize(lightVec);
	float diffuseFactor = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuse = lightColor * (diffuseFactor * albedo);

	// specular lighting
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(lightDirection, normal);

	float specAmount = pow(max(dot(viewDirection, reflectionDirection), dotFactor), roughness*roughnessFactor);
	vec3 specular = lightColor * specAmount;

	vec3 finalColor = (ambient + diffuse + specular) * inten;
	vec3 gammaCorrectedColor = pow(finalColor, vec3(1.0f / 2.2f));

	//vec4 light = texture(lightBuffer, texCoord);
	//vec4 godRays = (light.rgb, 1.0) * 2.0;

	vec4 color = vec4(gammaCorrectedColor, 1.0);

	return color;
}

vec4 pointLight2(vec4 lightColor, vec3 lightPos, float a, float b)
{
	vec3 lightVec = lightPos - crntPos;

	float dist = length(lightVec);
	float inten = 5.0f / (a * dist * dist + b * dist + 1.0f);

	vec4 ambient = lightColor * 0.4f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 0.3f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);

	vec3 halfwayVector = normalize(viewDirection + lightDirection);
	float specAmount = pow(max(dot(normal, halfwayVector), 0.0f), 16);
	float specular = specAmount * specularLight;


	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}
vec4 directLight(vec4 lightColor)
{	
	// ambient lighting
	vec4 ambient = lightColor * 0.50f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);

	vec3 halfwayVector = normalize(viewDirection + lightDirection);


	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
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
	vec4 pointLight0 = pointLight(lightColor0, lightPos0, 1.0, 0.5, dotFactor, roughnessFactor);
	vec4 pointLight1 = pointLight2(lightColor1, lightPos1, 1.0, 0.5);
	vec4 directLight = directLight(lightColorDirect);
	float fogFactor = calculateFog(gl_FragCoord.z);
	FragColor = mix(pointLight0, vec4(fogColor, 1.0), fogFactor) + pointLight1 + directLight;
}