#include "cMaterial.h"

Material::Material()
{
	mSpecularIntensity = 0;
	mShininess = 0;
}

Material::Material(GLfloat specIntensity, GLfloat shineFactor)
{
	mSpecularIntensity = specIntensity;
	mShininess = shineFactor;
}

Material::~Material()
{
}

void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
	glUniform1f(specularIntensityLocation, mSpecularIntensity);
	glUniform1f(shininessLocation, mShininess);
}
