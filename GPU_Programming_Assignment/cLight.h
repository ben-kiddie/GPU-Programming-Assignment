#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, 
		GLfloat ambIntensity, GLfloat diffIntensity);
	~Light();

protected:
	glm::vec3 mColour;
	GLfloat mAmbientIntensity, mDiffuseIntensity;
};

