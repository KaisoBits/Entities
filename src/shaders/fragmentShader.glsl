#version 460 core

out vec4 FragColor;

in vec2 textureCoord;
in float sunStrength;

uniform vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
uniform sampler2D texture0;

uniform vec3 sunColor = vec3(0.96, 0.93, 0.8);

void main()
{
	vec3 ambientLight = vec3(0.07);
	vec3 diffuse = sunStrength * sunColor;

	vec4 light = vec4(ambientLight + diffuse, 1.0);

	FragColor = texture(texture0, vec2(textureCoord.x, textureCoord.y)) * light;
}