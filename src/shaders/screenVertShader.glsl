#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 textureCoords;

void main()
{
	textureCoords = uv;
	gl_Position = vec4(position.xy, 0.0, 1.0);
}