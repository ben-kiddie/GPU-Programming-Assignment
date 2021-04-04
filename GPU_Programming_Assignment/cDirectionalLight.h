#pragma once

#include "cLight.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat ambIntensity, GLfloat diffIntensity,
		GLfloat xDirection, GLfloat yDirection, GLfloat zDirection);
	~DirectionalLight();

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
		GLuint diffuseIntensityLocation, GLuint directionLocation);


private:
	glm::vec3 mDirection;
};

