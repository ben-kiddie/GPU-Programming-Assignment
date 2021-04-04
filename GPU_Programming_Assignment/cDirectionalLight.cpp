#include "cDirectionalLight.h"

DirectionalLight::DirectionalLight() 
	: Light()
{
	mDirection = glm::vec3(0.0f, -1.0f, 0.0f);
}

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
	GLfloat ambIntensity, GLfloat diffIntensity,
	GLfloat xDirection, GLfloat yDirection, GLfloat zDirection) 
	: Light(red, green, blue, ambIntensity, diffIntensity)
{
	mDirection = glm::vec3(xDirection, yDirection, zDirection);
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
	GLuint diffuseIntensityLocation, GLuint directionLocation)
{
	glUniform3f(ambientColourLocation, mColour.x, mColour.y, mColour.z);
	glUniform1f(ambientIntensityLocation, mAmbientIntensity);
	glUniform3f(directionLocation, mDirection.x, mDirection.y, mDirection.z);
	glUniform1f(diffuseIntensityLocation, mDiffuseIntensity);
}