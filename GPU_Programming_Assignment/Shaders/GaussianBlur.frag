#version 330 core

// Kernels / image filtering (further reading):
//	https://en.wikipedia.org/wiki/Kernel_(image_processing) - Examples of some kernels to use
//	https://www.codingame.com/playgrounds/2524/basic-image-manipulation/filtering - Good explanation of the image filtering process and weightings for kernels
//	https://setosa.io/ev/image-kernels/ - Solid explanation of image filtering with examples of images before and after
//	https://docs.isf.video/primer_chapter_6.html#what-is-a-convolution-matrix - even more explanation here, with additional weighting examples

in vec2 textureCoordinates;

out vec4 fragColour;

uniform sampler2D screenTexture;

void main()
{
	// Identify the width of the s-axis (i.e., x-axis in the range 0-1), and height of the t-axis (i.e., y-axis also between 0-1)
	vec2 textureResolution = textureSize(screenTexture, 0);
	float stepS = 1.0f/textureResolution.x;
	float stepT = 1.0f/textureResolution.y;

	// This 5x5 kernel will hold the weightings for which neighbouring pixels have on the one we are sampling.
	float kernel5x5[25];
	kernel5x5[0] = 1.0f,	kernel5x5[1] = 4.0f,	kernel5x5[2] = 6.0f,	kernel5x5[3] = 4.0f,	kernel5x5[4] = 1.0f,
	kernel5x5[5] = 4.0f,	kernel5x5[6] = 16.0f,	kernel5x5[7] = 24.0f,	kernel5x5[8] = 16.0f,	kernel5x5[9] = 4.0f,
	kernel5x5[10] = 6.0f,	kernel5x5[11] = 24.0f,	kernel5x5[12] = 36.0f,	kernel5x5[13] = 24.0f,	kernel5x5[14] = 6.0f,
	kernel5x5[15] = 4.0f,	kernel5x5[16] = 16.0f,	kernel5x5[17] = 24.0f,	kernel5x5[18] = 16.0f,	kernel5x5[19] = 4.0f,
	kernel5x5[20] = 1.0f,	kernel5x5[21] = 4.0f,	kernel5x5[22] = 6.0f,	kernel5x5[23] = 4.0f,	kernel5x5[24] = 1.0f;

	// Array of pre-calculated steps to quickly find neighbouring pixels
	vec2 offsetLUT[25];
	offsetLUT[0] = vec2(-2*stepS, -2*stepT),	offsetLUT[1] = vec2(-stepS, -2*stepT),	offsetLUT[2] = vec2(0.0f, -2*stepT),	offsetLUT[3] = vec2(stepS, -2*stepT),	offsetLUT[4] = vec2(2*stepS, -2*stepT),
	offsetLUT[5] = vec2(-2*stepS, -stepT),		offsetLUT[6] = vec2(-stepS, -stepT),	offsetLUT[7] = vec2(0.0f, -stepT),		offsetLUT[8] = vec2(stepS, -stepT),		offsetLUT[9] = vec2(2*stepS, -stepT),
	offsetLUT[10] = vec2(-2*stepS, 0.0f),		offsetLUT[11] = vec2(-stepS, 0.0f),		offsetLUT[12] = vec2(0.0f, 0.0f),		offsetLUT[13] = vec2(stepS, 0.0f),		offsetLUT[14] = vec2(2*stepS, 0.0f),
	offsetLUT[15] = vec2(-2*stepS, stepT),		offsetLUT[16] = vec2(-stepS, stepT),	offsetLUT[17] = vec2(0.0f, stepT),		offsetLUT[18] = vec2(stepS, stepT),		offsetLUT[19] = vec2(2*stepS, stepT),
	offsetLUT[20] = vec2(-2*stepS, 2*stepT),	offsetLUT[21] = vec2(-stepS, 2*stepT),	offsetLUT[22] = vec2(0.0f, 2*stepT),	offsetLUT[23] = vec2(stepS, 2*stepT),	offsetLUT[24] = vec2(2*stepS, 2*stepT);

	vec4 convolvedPixel = vec4(0.0f, 0.0f, 0.0f, 1.0f);	// Stores the fragment resulting from applying the gaussian blur filter kernel
	for(int i = 0; i < 25; i++)
	{
		vec4 tempVec = vec4(texture(screenTexture, textureCoordinates + offsetLUT[i]).rgb, 1.0f);
		convolvedPixel += (tempVec * kernel5x5[i]);
	}
	fragColour = convolvedPixel / 256;
}