#version 460 core

out vec4 FragColor;

in vec2 textureCoord;
in vec3 fragmentPosition;
in vec3 normalVector;

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

struct Sun
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Sun sun;

struct PointLight
{
	vec3 position;

	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform PointLight pointLight;

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

float getAttenuation(float dist)
{
	return 1.0 / (pointLight.constant + 
	pointLight.linear * dist + 
	pointLight.quadratic * dist * dist);
}

void main()
{
	// Sample material
	vec3 diffuseMaterialStrength = material.diffuseOverride ? 
		material.color : vec3(texture(material.diffuseMap, textureCoord)) * material.color;
	vec3 specularMaterialStrength = material.specularOverride ? 
		vec3(0.5) : vec3(texture(material.specularMap, textureCoord));

	// Sun light
	vec3 ambientLight = sun.ambient * diffuseMaterialStrength;
	vec3 diffuseLight = sun.diffuse * getDiffuseLightStrength(sun.direction) * diffuseMaterialStrength;
	vec3 specularLight = sun.specular * getSpecularLightStrength(sun.direction) * specularMaterialStrength;

	// Point light
	float distanceToPointLight = distance(fragmentPosition, pointLight.position);
	float attenuation = getAttenuation(distanceToPointLight);
	vec3 diffusePointLight = pointLight.diffuse * attenuation * 
		getDiffuseLightStrength(fragmentPosition - pointLight.position) * diffuseMaterialStrength;
	vec3 specularPointLight = pointLight.specular * attenuation * 
		getSpecularLightStrength(fragmentPosition - pointLight.position)  * specularMaterialStrength;

	vec3 sunLight = ambientLight + diffuseLight + specularLight;
	vec3 pointLight = diffusePointLight + specularPointLight;

	FragColor = vec4(sunLight + pointLight, 1.0);
}