#include "cPointLight.h"

PointLight::PointLight() 
	: Light()
{
	mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	mConstant = 1.0f;	// At the very least this must be initialised to 1, as the resulting light calculations demand the denominator not be equal to zero
	mLinear = 0.0f;
	mExponent = 0.0f;
}

PointLight::PointLight(GLfloat red, GLfloat green, GLfloat blue,
	GLfloat ambIntensity, GLfloat diffIntensity,
	GLfloat xPos, GLfloat yPos, GLfloat zPos,
	GLfloat constant, GLfloat linear, GLfloat exponent)
	: Light(red, green, blue, ambIntensity, diffIntensity)
{
	mPosition = glm::vec3(xPos, yPos, zPos);
	mConstant = constant;
	mLinear = linear;
	mExponent = exponent;
}

PointLight::~PointLight()
{
}

void PointLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation, 
	GLuint diffuseIntensityLocation, GLuint positionLocation, 
	GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation)
{
	glUniform3f(ambientColourLocation, mColour.x, mColour.y, mColour.z);
	glUniform1f(ambientIntensityLocation, mAmbientIntensity);
	glUniform1f(diffuseIntensityLocation, mDiffuseIntensity);

	glUniform3f(positionLocation, mPosition.x, mPosition.y, mPosition.z);
	glUniform1f(constantLocation, mConstant);
	glUniform1f(linearLocation, mLinear);
	glUniform1f(exponentLocation, mExponent);
}
