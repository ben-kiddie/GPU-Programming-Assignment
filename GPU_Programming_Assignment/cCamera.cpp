#include "cCamera.h"

Camera::Camera()
{
}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	mPosition = startPosition;
	mWorldUp = startUp;
	mYaw = startYaw;
	mPitch = startPitch;
	mFront = glm::vec3(0.0f, 0.0f, 1.0f);

	mMovementSpeed = startMoveSpeed;
	mTurnSpeed = startTurnSpeed;

	Update();
}

Camera::~Camera()
{
}

void Camera::KeyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = mMovementSpeed * deltaTime;

	if (keys[GLFW_KEY_W])
	{
		mPosition += mFront * velocity;
	}
	if (keys[GLFW_KEY_S])
	{
		mPosition -= mFront * velocity;
	}
	if (keys[GLFW_KEY_A])
	{
		mPosition -= mRight * velocity;
	}
	if (keys[GLFW_KEY_D])
	{
		mPosition += mRight * velocity;
	}
}

void Camera::MouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= mTurnSpeed;
	yChange *= mTurnSpeed;

	mYaw += xChange;
	mPitch += yChange;

	if (mPitch > 89.0f)
	{
		mPitch = 89.0f;
	}
	if (mPitch < -89.0f)
	{
		mPitch = -89.0f;
	}

	Update();
}

glm::mat4 Camera::CalculateViewMatrix()
{
	// Parameters of glm::lookAt:
	//	1 - The position of our camera or eye
	//	2 - The position of something we want to look towards
	//	3 - The local up vector relative to the camera
	return glm::lookAt(mPosition, mPosition + mFront, mUp);	// You can manipulate this to give a 3rd person effect. If we had a player we want to follow, set argument 1 to the position around our player, and argument 2 to the position of the player.
}

glm::vec3 Camera::GetCameraPosition()
{
	return mPosition;
}

glm::vec3 Camera::GetCameraDirection()
{
	return glm::normalize(mFront);
}

void Camera::Update()
{
	mFront.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mFront.y = sin(glm::radians(mPitch));
	mFront.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mFront = glm::normalize(mFront);

	mRight = glm::normalize(glm::cross(mFront, mWorldUp));

	mUp = glm::normalize(glm::cross(mRight, mFront));
}
