#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 model = mat4(1);
uniform mat4 view = mat4(1);
uniform mat4 perspective = mat4(1);

uniform vec3 sunDirectionVector = vec3(0.5, -1, 0);

out vec2 textureCoord;
out float sunStrength;

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
	textureCoord = uv;

	vec4 modelPos = model * vec4(position, 1.0);
	gl_Position = perspective * view * modelPos;

	vec3 normalRotationOnly = mat3(transpose(inverse(model))) * normal;

	float dotResult = dot(normalize(normalRotationOnly), normalize(sunDirectionVector));
	sunStrength = max(map(dotResult, -1, -0.1, 1.5, 0), 0);
}