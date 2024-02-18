#version 460 core

out vec4 FragColor;

in vec2 textureCoord;
in vec3 fragmentPosition;
in vec3 normalVector;

uniform vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

uniform sampler2D texture0;

uniform vec3 sunDirectionVector = vec3(0.5, -1, 0);
uniform vec3 sunColor = vec3(0.96, 0.93, 0.8);

uniform vec3 cameraPosition;

float map(float value, float min1, float max1, float min2, float max2) 
{
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float getDiffuseLightStrength() {
	float dotResult = dot(normalVector, normalize(sunDirectionVector));
	return max(map(dotResult, -1, -0.1, 1.5, 0), 0);
}

float getSpecularLightStrength() {
	float strength = 0.4;

	vec3 viewingDirection = normalize(cameraPosition - fragmentPosition);
	vec3 reflectDirection = reflect(normalize(sunDirectionVector), normalVector);
	float specLight = pow(max(dot(viewingDirection, reflectDirection), 0.0), 32);

	return specLight * strength;
}

void main()
{
	vec3 ambientLight = vec3(0.07);
	vec3 diffuse = sunColor * getDiffuseLightStrength();
	vec3 specular = sunColor * getSpecularLightStrength();

	vec4 light = vec4(ambientLight + diffuse + specular, 1.0);

	FragColor = texture(texture0, vec2(textureCoord.x, textureCoord.y)) * light;
}