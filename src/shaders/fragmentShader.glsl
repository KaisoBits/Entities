#version 460 core

out vec4 FragColor;

in vec2 textureCoord;
in vec3 fragmentPosition;
in vec3 normalVector;

uniform sampler2D texture0;

uniform vec3 cameraPosition;

struct Material
{
	vec3 ambient;
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

float getDiffuseLightStrength() {
	float dotResult = dot(normalVector, -normalize(sun.direction));
	return max(dotResult, 0);
}

float getSpecularLightStrength() {
	vec3 viewingDirection = normalize(cameraPosition - fragmentPosition);
	vec3 reflectDirection = reflect(normalize(sun.direction), normalVector);
	float specLight = pow(max(dot(viewingDirection, reflectDirection), 0.0), material.shininess);
	return specLight;
}

void main()
{
	vec3 ambientLight = sun.ambient * material.ambient;

	vec3 diffuseStrength = material.diffuseOverride ? 
		vec3(1) : vec3(texture(material.diffuseMap, textureCoord));
	vec3 diffuse = sun.diffuse * getDiffuseLightStrength() * diffuseStrength;

	vec3 specularStrength = material.specularOverride ? 
		vec3(0.5) : vec3(texture(material.specularMap, textureCoord));
	vec3 specular = sun.specular * getSpecularLightStrength() * specularStrength;

	vec4 light = vec4(ambientLight + diffuse + specular, 1.0);

	FragColor = texture(texture0, textureCoord) * light;
}