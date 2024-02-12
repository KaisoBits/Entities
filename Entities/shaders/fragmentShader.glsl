#version 460 core

out vec4 FragColor;

in vec2 textureCoord;

uniform vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
uniform sampler2D texture0;

void main()
{
	FragColor = texture(texture0, vec2(textureCoord.x, textureCoord.y));
}