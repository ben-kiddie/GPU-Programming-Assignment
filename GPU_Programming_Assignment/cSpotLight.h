#pragma once
#include "cPointLight.h"

class SpotLight : public PointLight
{
public:
    SpotLight();
    SpotLight(GLfloat red, GLfloat green, GLfloat blue,
        GLfloat ambIntensity, GLfloat diffIntensity,
        GLfloat xPos, GLfloat yPos, GLfloat zPos,
        GLfloat xDirection, GLfloat yDirection, GLfloat zDirection,
        GLfloat constant, GLfloat linear, GLfloat exponent,
        GLfloat edge);
    ~SpotLight();

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
        GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
        GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
        GLuint edgeLocation);

    void SetFlash(glm::vec3 position, glm::vec3 direction);

private:
	glm::vec3 mDirection;
	GLfloat mEdge, mProcessedEdge;	// The edge is our cut-off angle, where processed edge is our angle converted to radians
};

