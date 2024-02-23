#version 460 core

out vec4 FragColor;

uniform vec4 highlightColor = vec4(0.1, 1.0, 0.2, 1.0);

void main()
{
	FragColor = highlightColor;
}