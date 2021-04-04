#pragma once

#include "cLight.h"

class PointLight : public Light
{
public:
    PointLight();
    PointLight(GLfloat red, GLfloat green, GLfloat blue,
        GLfloat ambIntensity, GLfloat diffIntensity,
        GLfloat xPos, GLfloat yPos, GLfloat zPos,
        GLfloat constant, GLfloat linear, GLfloat exponent);
    ~PointLight();

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
        GLuint diffuseIntensityLocation, GLuint positionLocation,
        GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

protected:
    glm::vec3 mPosition;
    GLfloat mConstant, mLinear, mExponent;  // Used in the equation ax^2 + bx + c, where x is distance between light and fragment.
};

