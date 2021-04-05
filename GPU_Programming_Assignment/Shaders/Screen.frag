#version 330 core

in vec2 vTexCoord;

out vec4 vFragColour;

uniform sampler2D screenTexture;

void main()
{
	vec3 colour = texture(screenTexture, vTexCoord).rgb;

	vFragColour = vec4(colour, 1.0f);
}