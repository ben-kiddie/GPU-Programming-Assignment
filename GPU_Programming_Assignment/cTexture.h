#pragma once

#include <GL/glew.h>
#include <stb/stb_image.h>

// https://www.textures.com/ - texture repository

class Texture
{
public:
	Texture();
	Texture(const char* fileLocation);
	~Texture();

	/// <summary>
	/// Load a texture with RGB values.
	/// </summary>
	/// <returns>In the event this returns false, load a default texture.</returns>
	bool LoadTexture();
	/// <summary>
	/// Load a texture with RGBA values.
	/// </summary>
	/// <returns>In the event this returns false, load a default texture.</returns>
	bool LoadTextureA();

	void UseTexture();
	void ClearTexture();

private:
	GLuint mTextureID;
	int mWidth, mHeight, mBitDepth;
	const char* mFileLocation;
};

