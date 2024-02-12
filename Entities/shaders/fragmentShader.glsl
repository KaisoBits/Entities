#version 460 core

out vec4 FragColor;

in vec2 textureCoord;

uniform vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float time;

void main()
{
	float zeroToOne = ((sin(time) + 1.0) / 2.0);
	float multiplier = zeroToOne * 0.5 + 0.5;

	FragColor = mix(texture(texture0, vec2(textureCoord.x, textureCoord.y)), texture(texture1, vec2(textureCoord.x, textureCoord.y)), zeroToOne);
}