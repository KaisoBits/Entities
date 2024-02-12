#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 model = mat4(1);
uniform mat4 view = mat4(1);
uniform mat4 perspective = mat4(1);

out vec2 textureCoord;

void main()
{
	gl_Position = perspective * view * model * vec4(position, 1.0);
	textureCoord = uv;
}