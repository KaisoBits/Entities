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

struct SpotLight
{
	vec3 position;
	vec3 direction;

	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutoff;
};
uniform SpotLight spotLight;

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
	float pointLightAttenuation = getAttenuation(distanceToPointLight);
	vec3 diffusePointLight = pointLight.diffuse * pointLightAttenuation  * 
		getDiffuseLightStrength(fragmentPosition - pointLight.position) * diffuseMaterialStrength;
	vec3 specularPointLight = pointLight.specular * pointLightAttenuation  * 
		getSpecularLightStrength(fragmentPosition - pointLight.position)  * specularMaterialStrength;

	// Spot light
	float distanceToSpotLight = distance(fragmentPosition, spotLight.position);
	float spotLightAttenuation = getAttenuation(distanceToSpotLight);
	vec3 diffuseSpotLight = spotLight.diffuse * spotLightAttenuation * 
		getDiffuseLightStrength(fragmentPosition - spotLight.position) * diffuseMaterialStrength;
	vec3 specularSpotLight = spotLight.specular * spotLightAttenuation * 
		getSpecularLightStrength(fragmentPosition - spotLight.position)  * specularMaterialStrength;

	vec3 toSpotLight = normalize(spotLight.position - fragmentPosition);
	float angle = dot(toSpotLight, -normalize(spotLight.direction));
	float spotLighAngleModifier = angle < spotLight.cutoff ? 0.0 : 1.0;

	// Combine
	vec3 sunLight = ambientLight + diffuseLight + specularLight;
	vec3 pointLight = diffusePointLight + specularPointLight;
	vec3 spotLight = (diffuseSpotLight + specularSpotLight) * spotLighAngleModifier;

	FragColor = vec4(sunLight + pointLight + spotLight, 1.0);
}