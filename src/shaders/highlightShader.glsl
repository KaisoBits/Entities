#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntitiyId;

uniform vec4 highlightColor = vec4(0.1, 1.0, 0.2, 1.0);

void main()
{
	FragColor = highlightColor;
	EntitiyId = -1;
}