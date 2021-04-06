#version 330 core

// Input
in vec2 vTexCoord;

// Output		
out vec4 fragColour;

uniform sampler2D screenTexture;

void main()
{
	// Identify the width of the s-axis (i.e., x-axis in the range 0-1), and height of the t-axis (i.e., y-axis also between 0-1)
	vec2 textureResolution = textureSize(screenTexture, 0);
	float stepS = 1.0f/textureResolution.x;
	float stepT = 1.0f/textureResolution.y;

	// This 3x3 kernel will hold the weightings for which neighbouring pixels have on the one we are sampling.
	float kernel3x3[9];
	kernel3x3[0] = 1.0f,	kernel3x3[1] = 1.0f,	kernel3x3[2] = 1.0f,
	kernel3x3[3] = 1.0f,	kernel3x3[4] = 1.0f,	kernel3x3[5] = 1.0f,
	kernel3x3[6] = 1.0f,	kernel3x3[7] = 1.0f,	kernel3x3[8] = 1.0f;

	// Array of pre-calculated steps to quickly find neighbouring pixels
	vec2 offsetLUT[9];
	offsetLUT[0] = vec2(-stepS, -stepT),	offsetLUT[1] = vec2(0.0f, -stepT),	offsetLUT[2] = vec2(stepS, -stepT),
	offsetLUT[3] = vec2(-stepS, 0.0f),		offsetLUT[4] = vec2(0.0f, 0.0f),	offsetLUT[5] = vec2(stepS, 0.0f),
	offsetLUT[6] = vec2(-stepS, stepT),		offsetLUT[7] = vec2(0.0f, stepT),	offsetLUT[8] = vec2(stepS, stepT);

	vec4 convolvedPixel = vec4(0.0f, 0.0f, 0.0f, 1.0f);	// Stores the fragment resulting from applying the box blur filter kernel
	for(int i = 0; i < 9; i++)
	{
		vec4 tempVec = vec4(texture(screenTexture, vTexCoord + offsetLUT[i]).rgb, 1.0f);
		convolvedPixel += (tempVec * kernel3x3[i]);
	}
	fragColour = convolvedPixel / 9;	// Normalise fragment through dividing by kernel size
}