#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);
	~Camera();

	void KeyControl(bool* keys, GLfloat deltaTime);
	void MouseControl(GLfloat xChange, GLfloat yChange);

	glm::mat4 CalculateViewMatrix();

	glm::vec3 GetCameraPosition();
	glm::vec3 GetCameraDirection();

private:
	glm::vec3 mPosition,
		mFront,
		mUp,
		mRight,
		mWorldUp;

	GLfloat mYaw,
		mPitch,
		mMovementSpeed,
		mTurnSpeed;

	void Update();
};

