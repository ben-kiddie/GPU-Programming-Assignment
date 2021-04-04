#include "cTexture.h"

Texture::Texture()
{
	mTextureID = 0;
	mWidth = 0;
	mHeight = 0;
	mBitDepth = 0;
	mFileLocation = "";
}

Texture::Texture(const char* fileLocation)
{
	mTextureID = 0;
	mWidth = 0;
	mHeight = 0;
	mBitDepth = 0;
	mFileLocation = fileLocation;
}

Texture::~Texture()
{
	ClearTexture();
}

bool Texture::LoadTexture()
{
	unsigned char* texData = stbi_load(mFileLocation, &mWidth, &mHeight, &mBitDepth, 0);	// 1 char is equal to 1 byte, which is commonly used to represent data in files	
	if (!texData)
	{
		printf("Failed to find: %s\n", mFileLocation);
		return false;
	}

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);	// Fun fact: 2d textures are, as you guessed, flat textures. 3d textures can be used in things like clouds, where if you are inside it, you have textures in a 3d setting, and not just a 2d texture plastered onto something.

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);	// If your texture comes out looking strangely, check the format - you may want GL_RGBA in some cases
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(texData);
	return true;
}

bool Texture::LoadTextureA()
{
	unsigned char* texData = stbi_load(mFileLocation, &mWidth, &mHeight, &mBitDepth, 0);
	if (!texData)
	{
		printf("Failed to find: %s\n", mFileLocation);
		return false;
	}

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);	// If your texture comes out looking strangely, check the format - you may want GL_RGB in some cases
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(texData);
	return true;
}

void Texture::UseTexture()
{
	glActiveTexture(GL_TEXTURE0);	// Pass in the texture unit we want to use. The sampler will access a texture through the unit we identify. We can cycle through textures accessed in the fragment shader by activating different units, usually up to 32 units.
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture::ClearTexture()
{
	glDeleteTextures(1, &mTextureID);
	mTextureID = 0;
	mWidth = 0;
	mHeight = 0;
	mBitDepth = 0;
	mFileLocation = "";
}
