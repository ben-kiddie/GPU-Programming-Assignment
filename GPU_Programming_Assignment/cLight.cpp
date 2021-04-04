#include "cLight.h"

Light::Light()
{
	mColour = glm::vec3(1.0f, 1.0f, 1.0f);
	mAmbientIntensity = 1.0f;
	mDiffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, 
	GLfloat ambIntensity, GLfloat diffIntensity)
{
	mColour = glm::vec3(red, green, blue);
	mAmbientIntensity = ambIntensity;
	mDiffuseIntensity = diffIntensity;
}

Light::~Light()
{
}
