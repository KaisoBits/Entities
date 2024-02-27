#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntitiyId;

in vec2 textureCoord;
in vec3 fragmentPosition;
in vec3 normalVector;

uniform int entityId;

uniform vec3 cameraPosition;

struct Material
{
	vec3 color;
	bool diffuseOverride;
	sampler2D diffuseMap;
	bool specularOverride;
	sampler2D specularMap;
	float shininess;
};
uniform Material material;

#define MAX_LIGHTS 10

struct Sun
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Sun suns[MAX_LIGHTS];
uniform int sunsCount = 0;

struct PointLight
{
	vec3 position;

	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform PointLight pointLights[MAX_LIGHTS];
uniform int pointLightsCount = 0;

struct SpotLight
{
	vec3 position;
	vec3 direction;

	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float innerCutoff;
	float outerCutoff;
};
uniform SpotLight spotLights[MAX_LIGHTS];
uniform int spotLightsCount = 0;

float getDiffuseLightStrength(vec3 lightDirection) {
	float dotResult = dot(normalVector, -normalize(lightDirection));
	return max(dotResult, 0);
}

float getSpecularLightStrength(vec3 lightDirection) {
	vec3 viewingDirection = normalize(cameraPosition - fragmentPosition);
	vec3 reflectDirection = reflect(normalize(lightDirection), normalVector);
	float specLight = pow(max(dot(viewingDirection, reflectDirection), 0.0), material.shininess);
	return specLight;
}

float getAttenuation(float dist, float constant, float linear, float quadratic)
{
	return 1.0 / (constant + linear * dist + quadratic * dist * dist);
}

void main()
{
	// Sample material
	vec4 pixelColor = texture(material.diffuseMap, textureCoord);
	if (pixelColor.w < 0.1)
		discard;

	vec3 diffuseMaterialStrength = material.diffuseOverride ? 
		material.color : vec3(pixelColor) * material.color;
	vec3 specularMaterialStrength = material.specularOverride ? 
		vec3(0.5) : vec3(texture(material.specularMap, textureCoord));

	// Sun light
	vec3 ambientLight = vec3(0);
	vec3 diffuseLight = vec3(0);
	vec3 specularLight = vec3(0);
	for (int i = 0; i < sunsCount && i < MAX_LIGHTS; i++)
	{
		ambientLight += suns[i].ambient * diffuseMaterialStrength;
		diffuseLight += suns[i].diffuse * getDiffuseLightStrength(suns[i].direction) * diffuseMaterialStrength;
		specularLight += suns[i].specular * getSpecularLightStrength(suns[i].direction) * specularMaterialStrength;
	}

	// Point light
	vec3 diffusePointLight = vec3(0);
	vec3 specularPointLight = vec3(0);
	for (int i = 0; i < pointLightsCount && i < MAX_LIGHTS; i++)
	{
		float distanceToPointLight = distance(fragmentPosition, pointLights[i].position);
		float pointLightAttenuation = getAttenuation(distanceToPointLight, pointLights[i].constant, pointLights[i].linear, pointLights[i].quadratic);
		diffusePointLight += pointLights[i].diffuse * pointLightAttenuation * 
			getDiffuseLightStrength(fragmentPosition - pointLights[i].position) * diffuseMaterialStrength;
		specularPointLight += pointLights[i].specular * pointLightAttenuation  * 
			getSpecularLightStrength(fragmentPosition - pointLights[i].position)  * specularMaterialStrength;
	}

	// Spot light
	vec3 diffuseSpotLight = vec3(0);
	vec3 specularSpotLight = vec3(0);
	for (int i = 0; i < spotLightsCount && i < MAX_LIGHTS; i++)
	{
		float distanceToSpotLight = distance(fragmentPosition, spotLights[i].position);
		float spotLightAttenuation = getAttenuation(distanceToSpotLight, spotLights[i].constant, spotLights[i].linear, spotLights[i].quadratic);

		vec3 toSpotLight = normalize(spotLights[i].position - fragmentPosition);
		float angle = dot(toSpotLight, -normalize(spotLights[i].direction));
		float spotLighAngleModifier = clamp((angle - spotLights[i].outerCutoff) / (spotLights[i].innerCutoff - spotLights[i].outerCutoff), 0, 1);

		diffuseSpotLight += spotLights[i].diffuse * spotLightAttenuation * 
			getDiffuseLightStrength(fragmentPosition - spotLights[i].position) * diffuseMaterialStrength * spotLighAngleModifier;
		specularSpotLight += spotLights[i].specular * spotLightAttenuation * 
			getSpecularLightStrength(fragmentPosition - spotLights[i].position)  * specularMaterialStrength * spotLighAngleModifier;
	}

	// Combine
	vec3 sunLights = ambientLight + diffuseLight + specularLight;
	vec3 pointLights = diffusePointLight + specularPointLight;
	vec3 spotLights = diffuseSpotLight + specularSpotLight;

	FragColor = vec4(sunLights + pointLights + spotLights, 1.0);
	EntitiyId = entityId;
}