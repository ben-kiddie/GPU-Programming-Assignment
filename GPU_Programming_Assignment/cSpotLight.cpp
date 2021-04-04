#include "cSpotLight.h"

SpotLight::SpotLight()
	: PointLight()
{
	mDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	mEdge = 0.0f;
	mProcessedEdge = cosf(glm::radians(mEdge));
}

SpotLight::SpotLight(GLfloat red, GLfloat green, GLfloat blue, 
	GLfloat ambIntensity, GLfloat diffIntensity, 
	GLfloat xPos, GLfloat yPos, GLfloat zPos, 
	GLfloat xDirection, GLfloat yDirection, GLfloat zDirection, 
	GLfloat constant, GLfloat linear, GLfloat exponent, 
	GLfloat edge)
	: PointLight(red, green, blue, ambIntensity, diffIntensity, xPos, yPos, zPos, constant, linear, exponent)
{
	mDirection = glm::normalize(glm::vec3(xDirection, yDirection, zDirection));
	mEdge = edge;
	mProcessedEdge = cosf(glm::radians(mEdge));
}

SpotLight::~SpotLight()
{
}

void SpotLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation, 
	GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation, 
	GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation, 
	GLuint edgeLocation)
{
	glUniform3f(ambientColourLocation, mColour.x, mColour.y, mColour.z);
	glUniform1f(ambientIntensityLocation, mAmbientIntensity);
	glUniform1f(diffuseIntensityLocation, mDiffuseIntensity);

	glUniform3f(positionLocation, mPosition.x, mPosition.y, mPosition.z);
	glUniform1f(constantLocation, mConstant);
	glUniform1f(linearLocation, mLinear);
	glUniform1f(exponentLocation, mExponent);

	glUniform3f(directionLocation, mDirection.x, mDirection.y, mDirection.z);
	glUniform1f(edgeLocation, mProcessedEdge);
}

void SpotLight::SetFlash(glm::vec3 position, glm::vec3 direction)
{
	mPosition = position;
	mDirection = direction;
}
