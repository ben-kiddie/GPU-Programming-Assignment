#pragma once

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	~Window();

	int Initialise();

	GLfloat GetBufferWidth() { return bufferWidth; }
	GLfloat GetBufferHeight() { return bufferHeight; }

	bool GetShouldClose() { return glfwWindowShouldClose(mainWindow); }

	bool* GetKeys() { return keys; }

	// In these functions, as soon as we return the change values, we reset the last value to zero. In practice, this is a poor solution, but works for now.
	GLfloat GetXChange();
	GLfloat GetYChange();

	void SwapBuffers() { glfwSwapBuffers(mainWindow); }

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	GLint bufferWidth, bufferHeight;

	bool keys[1024];	// Covers all ASCII characters - so when we press a key, the corresponding element in the array is set to true, and set to false when unpressed

	// NOTE: If we don't move our mouse at all, but have done so previously, lastX and lastY are not reset to 0 - this means the change in x and y, even while the cursor is stationary, will feedback incorrect input from the user. In most games, these values should be reset to 0 as soon as the user stops moving their cursor.
	GLfloat lastX, lastY,	// Last x and y update every frame, reflecting the mouse position during the previous frame
		xChange, yChange;	// x and y change are found by getting the current frame mouse coordinates and subtracting the last coordinates
	bool mouseFirstMoved;	// Check if this is the first movement. We don't want to check during scenarios such as when the application first starts and our mouse is idling at the extremities of our window, causing a sudden drastic movement.

	void CreateCallbacks();
	static void HandleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void HandleMouse(GLFWwindow* window, double xPos, double yPos);
};

