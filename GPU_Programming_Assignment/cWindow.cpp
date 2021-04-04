#include "cWindow.h"

Window::Window()
{
	width = 800, height = 600;
	xChange = 0.0f, yChange = 0.0f;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth, height = windowHeight;
	xChange = 0.0f, yChange = 0.0f;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);	
	glfwTerminate();	
}

int Window::Initialise()
{
	// Initialise GLFW
	if (!glfwInit())	// glfwInit returns "GLFW_TRUE" if successful, "GLFW_FALSE" if unsuccessful in setting up a context.
	{
		printf("GLFW initialisation failed!");	// If the check returns GLFW_FALSE, we know our window creation failed, so output to console window
		glfwTerminate();	// Cleanup after GLFW right before the program terminates
		return 1;	// Returning '1' is the standard way in c++ of returning an error when closing a program
	}

	// Setup GLFW window properties
	// OpenGL version
	// glfwWindowHint - Setting certain properties of the GLFW window we create
	// Parameters of glfwWindowHint:
	//	1 - An enumerator describing the property we want to edit
	//	2 - The value for the property we want to set
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// We set context for our OpenGL major version to 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// We set the minor version to 3, effectively saying we are using OpenGL version 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // The profile describes how OpenGL is going to treat code its passed - core profile means it is not backwards compatible, and will not use old, deprecated, or potential redundant features, and throw up an error if it coems across some
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Allow forward compatibility

	// Create a window and have a pointer to the window in memory
	// Parameters of glfwCreateWindow:
	//	1 - Width of the window
	//	2 - Height of the window
	//	3 - Title of our window
	//	4 - The monitor to display to
	//	5 - Window sharing across systems
	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);	// Create window using the dimensions specified in the WIDTH and HEIGHT variables. Change the name as necessary, though we won't use the other properties for now.	
	if (!mainWindow)	// In the event a window fails to create, print an error message, clean up glfw, and return program error
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	// Parameters of glfwGetFrameBufferSize:
	//	1 - A pointer to the window we want to use
	//	2 - Reference to the memory to store buffer width
	//	3 - Reference to memory to store buffer height
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);	// Get the buffer size from our main window and store data in our buffers

	// Set the context for GLEW to use - i.e., the window context to attach to and modify. You can swap these as frequently as you want, but GLFW will interface with the current.
	// Parameters of glfwMakeContextCurrent:
	//	1 - A pointer to the window which we want to make the context
	glfwMakeContextCurrent(mainWindow);	// We setup our main window as our context

	// Handle key and mouse input
	CreateCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// This disables our cursor so we cannot see it in our window, but it acts as if it is always centered on our window

	// Allow modern extension features
	glewExperimental = GL_TRUE;	// Enables access to extensions for experimental features. A bit too advanced for this course, but no downsides to enabling.

	// Enable GLEW - glewInit also lets us check for errors
	if (glewInit() != GLEW_OK)	// glewInit returns an enum describing a successful or failed initialisation. GLEW_OK is the state of successful init.
	{
		printf("GLEW initialisation failed!");	// Print error message for GLEW fail
		glfwDestroyWindow(mainWindow);	// Since we have a window setup, but GLEW doesn't work, for cleanup first destroy the window made by GLFW
		glfwTerminate();	// After window destroyed, THEN terminate GLFW
		return 1;	// Return an error, as always
	}

	glEnable(GL_DEPTH_TEST);

	// Setup viewport size (i.e., the  - here is where we begin using OpenGL things!
	// Parameters of glViewport:
	//	1 - Top left x coordinate
	//	2 - Top left y coordinate
	//	3 - Width of the window space we draw with (i.e., our buffers)
	//	4 - Height of our window space to draw
	glViewport(0, 0, bufferWidth, bufferHeight);	// Setup our viewport with pre-defined buffer sizes

	glfwSetWindowUserPointer(mainWindow, this);
}

GLfloat Window::GetXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::GetYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::CreateCallbacks()
{
	// When a key is pressed in mainwindow, callback to the handle keys function and pass in all the variables
	glfwSetKeyCallback(mainWindow, HandleKeys);

	glfwSetCursorPosCallback(mainWindow, HandleMouse);
}

void Window::HandleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
		}
	}
}

void Window::HandleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	// Update x and y - note that the y is inverted to replicate what games usually use. To invert y controls, subtract ypos from lasty
	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}
