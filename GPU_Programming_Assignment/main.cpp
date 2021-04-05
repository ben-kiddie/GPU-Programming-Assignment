// Definitions
#define STB_IMAGE_IMPLEMENTATION

// Standard includes
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

// External includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <assimp/Importer.hpp>

// Custom classes
#include "Globals.h"
#include "cMesh.h"
#include "cShader.h"
#include "cWindow.h"
#include "cCamera.h"
#include "cTexture.h"
#include "cDirectionalLight.h"
#include "cPointLight.h"
#include "cSpotLight.h"
#include "cMaterial.h"
#include "cModel.h"

std::vector<Mesh*> meshList;

void CreateMeshes()
{
	unsigned int leftScreenQuadIndices[] = {
		0,	1,	2,
		2,	3,	0
	};
	GLfloat leftScreenQuadVertices[] = {
	//	x		y		z		u		v		nX		nY		nZ
		-1.0f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	// Top left
		-1.0f,	-1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	// Bottom left
		0.0f,	-1.0f,	0.0f,	0.5f,	0.0f,	0.0f,	0.0f,	0.0f,	// Bottom middle
		0.0f,	1.0f,	0.0f,	0.5f,	1.0f,	0.0f,	0.0f,	0.0f	// Top middle
	};
	Mesh* leftScreenQuad = new Mesh();
	leftScreenQuad->CreateMesh(leftScreenQuadVertices, leftScreenQuadIndices, 32, 6);
	meshList.push_back(leftScreenQuad);

	unsigned int rightScreenQuadIndices[] = {
		0,	1,	2,
		2,	3,	0
	};
	GLfloat rightScreenQuadVertices[] = {
	//	x		y		z		u		v		nX		nY		nZ
		0.0f,	1.0f,	0.0f,	0.5f,	1.0f,	0.0f,	0.0f,	0.0f,	// Top middle
		0.0f,	-1.0f,	0.0f,	0.5f,	0.0f,	0.0f,	0.0f,	0.0f,	// Bottom middle
		1.0f,	-1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	// Bottom right
		1.0f,	1.0f,	0.0f,	1.0f,	1.0f,	0.0f,	0.0f,	0.0f	// Top right
	};
	Mesh* rightScreenQuad = new Mesh();
	rightScreenQuad->CreateMesh(rightScreenQuadVertices, rightScreenQuadIndices, 32, 6);
	meshList.push_back(rightScreenQuad);
}

int main()
{
#pragma region Local Variables
	// Window
	Window mainWindow;
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.2f);
	GLfloat deltaTime = 0.0f, lastTime = 0.0f;

	// Models
	Model floorOBJ;
	Model chopperOBJ;

	// Materials
	Material shinyMaterial = Material(4.0f, 156);
	Material dullMaterial = Material(0.3f, 4);

	// Lights
	DirectionalLight mainLight;
	PointLight pointLights[MAX_POINT_LIGHTS];
	SpotLight spotLights[MAX_SPOT_LIGHTS];

	// Shaders
	Shader blinnPhongShader, defaultScreenShader, sharpenShader, boxBlurShader;

	// Transformations
	const float toRadians = 3.14159265f / 180.0f;	// If we multiply a floating point angle (in degrees) by this value, it will output a radian value
	float chopperYRotation = 0.0f;	// Increment or decrement each frame to rotate the chopper
#pragma endregion

	mainWindow = Window(SCREEN_WIDTH, SCREEN_HEIGHT);
	mainWindow.Initialise();

	CreateMeshes();

	floorOBJ.LoadModel("Models/plane.obj");
	chopperOBJ.LoadModel("Models/chopper.obj");

	blinnPhongShader.CreateFromFiles("Shaders/shader.vert", "Shaders/shader.frag");
	defaultScreenShader.CreateFromFiles("Shaders/Screen.vert", "Shaders/Screen.frag");
	boxBlurShader.CreateFromFiles("Shaders/Screen.vert", "Shaders/BoxBlur.frag");
	sharpenShader.CreateFromFiles("Shaders/Screen.vert", "Shaders/Sharpen.frag");

#pragma region Create Framebuffers
	// Generate and bind a framebuffer
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Generate texture which captures our framebuffer
	unsigned int colourBuffer;
	glGenTextures(1, &colourBuffer);
	glBindTexture(GL_TEXTURE_2D, colourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	// Framebuffer texture size - resolution scale, i.e., width * height = 100% scale
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0);

	unsigned int renderBuffer;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	// Check framebuffer is correctly configured
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

	// Parameters of glm::perspective:
	//	1 - The angle for our FOV in the y axis
	//	2 - The aspect ratio, found by dividing screen width by screen height
	//	3 - The near plane, where anything in front of this is clipped
	//	4 - The far plane, where anything beyond this is clipped
	glm::mat4 projection = glm::perspective(45.0f, mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight(), 0.1f, 100.0f);	

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
								0.3f, 0.6f, 
								0.0f, 0.0f, -1.0f);

	unsigned int pointLightCount = 0;
	pointLightCount++;		// Red point light
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
								10.0f, 10.0f,
								-8.0f, 0.0f, 8.0f,
								0.3f, 0.2f, 0.2f);
	
	pointLightCount++;		// Green point light
	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
								10.0f, 10.0f,
								0.0f, 2.0f, -16.0f,
								0.3f, 0.2f, 0.2f);

	pointLightCount++;		// Blue point light
	pointLights[2] = PointLight(0.0f, 0.0f, 1.0f,
								10.0f, 10.0f,
								8.0f, 0.0f, 8.0f,
								0.3f, 0.2f, 0.2f);

	unsigned int spotLightCount = 0;
	spotLightCount++;		// Flash light
	spotLights[0] = SpotLight(	1.0f, 1.0f, 1.0f,
								0.0f, 0.5f,
								0.0f, 0.0f, 0.0f,
								0.0f, -1.0f, 0.0f,
								1.0f, 0.0f, 0.0f,
								20.0f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;

	while (!mainWindow.GetShouldClose())
	{
		// In this application, we are running in seconds. If we were to use SDL, we would be running in milliseconds, so we would need to adjust for this.
		GLfloat currentTime = glfwGetTime();
		GLfloat deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// Get and handle user input events
		glfwPollEvents();	// glfwPollEvents picks up events such as keyboard presses, mouse movements, clicking to close a window, moving a window, resizing a window, and more!
		bool* keys;
		keys = mainWindow.GetKeys();

		camera.KeyControl(mainWindow.GetKeys(), deltaTime);
		camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());

		// Bind our own framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		// Clear the window

		// Parameters of glClearColour:
		//	1 - Normalised red value
		//	2 - Normalised green value
		//	3 - Normalised blue value
		//	4 - Normalised alpha value
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);	// glClear clears a screen, ready for us to draw to a new frame. glClearColor lets us set the colour of our new frame, not just a black void! Remember the colour values you set should be normalised.

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// We use glClear to clear specific elements of our window. Pixels on screen contain more than just the colour - e.g., stencil data, depth data, and more. So we specify which to clear, as many as we want. In this case, we just clear all the colour buffers.
		glEnable(GL_DEPTH_TEST);

		blinnPhongShader.UseShader();
		uniformModel = blinnPhongShader.GetModelLocation();
		uniformProjection = blinnPhongShader.GetProjectionLocation();
		uniformView = blinnPhongShader.GetViewLocation();
		uniformEyePosition = blinnPhongShader.GetEyePositionLocation();
		uniformSpecularIntensity = blinnPhongShader.GetSpecularIntensityLocation();
		uniformShininess = blinnPhongShader.GetShininessLocation();
		
		glm::vec3 lowerLight = camera.GetCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.GetCameraDirection());

		blinnPhongShader.SetDirectionalLight(&mainLight);	// Note: the argument is a pointer, so we pass in the memory address
		blinnPhongShader.SetPointLights(pointLights, pointLightCount);
		blinnPhongShader.SetSpotLights(spotLights, spotLightCount);

		// View and projection only need to be setup once. Model varies among different objects, so we will setup just view and projection once.
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.CalculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.GetCameraPosition().x, camera.GetCameraPosition().y, camera.GetCameraPosition().z);	// Inside our fragment shader we want to know the eye position, i.e., camera pos
		glm::mat4 model(1.0f); // Setup a 4x4 identity matrix so that we can calculate using it later

		// Floor
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -4.0f, -4.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		floorOBJ.RenderModel();

		// Chopper
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		model = glm::rotate(model, chopperYRotation * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		chopperOBJ.RenderModel();
		chopperYRotation += 0.05f;

		glUseProgram(0);	// Once we're done with a shader program, remember to unbind it.

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		if (keys[49]) {
			sharpenShader.UseShader();
		}
		else {
			defaultScreenShader.UseShader();
		}
		glBindTexture(GL_TEXTURE_2D, colourBuffer);
		meshList[0]->RenderMesh();
		glUseProgram(0);

		if (keys[50]) {
			boxBlurShader.UseShader();
		}
		else {
			defaultScreenShader.UseShader();
		}
		glBindTexture(GL_TEXTURE_2D, colourBuffer);
		meshList[1]->RenderMesh();
		glUseProgram(0);

		mainWindow.SwapBuffers();
	}

	return 0;
}