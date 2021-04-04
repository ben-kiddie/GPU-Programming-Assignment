#pragma once

#include <GL\glew.h>

class Material
{
public:
	Material();
	Material(GLfloat specIntensity, GLfloat shineFactor);
	~Material();

	void UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

private:
	GLfloat mSpecularIntensity;
	GLfloat mShininess;	// The higher the shininess, the more concentrated and powerful reflected light will be
};

