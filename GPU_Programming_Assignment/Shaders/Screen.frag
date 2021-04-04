#version 330 core

in vec2 textureCoordinates;

out vec4 fragColour;

uniform sampler2D screenTexture;

void main()
{
	vec3 colour = texture(screenTexture, textureCoordinates).rgb;

	if(textureCoordinates.x <= 0.5f)
	{
		fragColour = vec4(colour, 1.0f) * vec4(0.1f, 0.3f, 0.1f, 1.0f);
	}
	else
	{
		fragColour = vec4(colour, 1.0f);
	}
}