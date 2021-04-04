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



const float toRadians = 3.14159265f / 180.0f;	// If we multiply a number by this value, it will output a radian value

// Vertex shader - to be moved to a separate file. Here we are taking in vertices to be modified or used as is, then passed to fragment shader.
static const char* vShader = "Shaders/shader.vert";

// Fragment shader - also to be moved to a separate file. Note: you don't normally pass anything in to the fragment shader, it simply picks up output from the vertex shader.
// Additional note - with the fragment shader, you don't even have to specify an out variable. If you only have one variable, it is assumed to be the colour, which is defaulted as an output.
static const char* fShader = "Shaders/shader.frag";

GLfloat deltaTime = 0.0f, lastTime = 0.0f;

Window mainWindow;
Camera camera;

Texture plainTexture, emeraldOreTexture, diamondOreTexture;
Material shinyMaterial, dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Model plane;
Model chopper;

std::vector<Shader> shaderList;



void CalculateAverageNormals(unsigned int* indices, unsigned int indexCount, GLfloat* vertices, unsigned int vertexCount, unsigned int vertexLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indexCount; i += 3)
	{
		// index0,1,2 stores the starting index in our vertices array for a vertex, given an index which points to it. Adding an offset will let us jump to a particular attribute, e.g., normals.
		unsigned int index0 = indices[i] * vertexLength;	// In our indices array, if we were to say point to index 3 of our vertices array, we know that the third vertex starts at index 9 (and follows on to index 10, then 11 for the full xyz).
		unsigned int index1 = indices[i + 1] * vertexLength;
		unsigned int index2 = indices[i + 2] * vertexLength;
		
		glm::vec3 v1(vertices[index1] - vertices[index0], vertices[index1 + 1] - vertices[index0 + 1], vertices[index1 + 2] - vertices[index0 + 2]);
		glm::vec3 v2(vertices[index2] - vertices[index1], vertices[index2 + 1] - vertices[index1 + 1], vertices[index2 + 2] - vertices[index1 + 2]);
	
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		index0 += normalOffset, index1 += normalOffset, index2 += normalOffset;

		vertices[index0] += normal.x, vertices[index0 + 1] += normal.y, vertices[index0 + 2] += normal.z;
		vertices[index1] += normal.x, vertices[index1 + 1] += normal.y, vertices[index1 + 2] += normal.z;
		vertices[index2] += normal.x, vertices[index2 + 1] += normal.y, vertices[index2 + 2] += normal.z;
	}

	for (size_t i = 0; i < vertexCount / vertexLength; i++)
	{
		unsigned int nOffset = i * vertexLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x, vertices[nOffset + 1] = vec.y, vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	plane.LoadModel("Models/plane.obj");
	chopper.LoadModel("Models/chopper.obj");
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.2f);

	// Parameters of glm::perspective:
	//	1 - The angle for our FOV in the y axis
	//	2 - The aspect ratio, found by dividing screen width by screen height
	//	3 - The near plane, where anything in front of this is clipped
	//	4 - The far plane, where anything beyond this is clipped
	glm::mat4 projection = glm::perspective(45.0f, mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight(), 0.1f, 100.0f);

	Assimp::Importer importer;	

	shinyMaterial = Material(4.0f, 156);
	dullMaterial = Material(0.3f, 4);

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

	float currentRotation = 0.0f;

	while (!mainWindow.GetShouldClose())
	{
		// In this application, we are running in seconds. If we were to use SDL, we would be running in milliseconds, so we would need to adjust for this.
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// Get and handle user input events
		glfwPollEvents();	// glfwPollEvents picks up events such as keyboard presses, mouse movements, clicking to close a window, moving a window, resizing a window, and more!

		camera.KeyControl(mainWindow.GetKeys(), deltaTime);
		camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());

		// Clear the window

		// Parameters of glClearColour:
		//	1 - Normalised red value
		//	2 - Normalised green value
		//	3 - Normalised blue value
		//	4 - Normalised alpha value
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// glClear clears a screen, ready for us to draw to a new frame. glClearColor lets us set the colour of our new frame, not just a black void! Remember the colour values you set should be normalised.

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// We use glClear to clear specific elements of our window. Pixels on screen contain more than just the colour - e.g., stencil data, depth data, and more. So we specify which to clear, as many as we want. In this case, we just clear all the colour buffers.

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();
		
		glm::vec3 lowerLight = camera.GetCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.GetCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);	// Note: the argument is a pointer, so we pass in the memory address
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// View and projection only need to be setup once. Model varies among different objects, so we will setup just view and projection once.
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.CalculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.GetCameraPosition().x, camera.GetCameraPosition().y, camera.GetCameraPosition().z);	// Inside our fragment shader we want to know the eye position, i.e., camera pos
		glm::mat4 model(1.0f); // Setup a 4x4 identity matrix so that we can calculate using it later

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -4.0f, -4.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		plane.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		model = glm::rotate(model, currentRotation * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		chopper.RenderModel();
		currentRotation += 0.05f;

		glUseProgram(0);	// Once we're done with a shader program, remember to unbind it.

		mainWindow.SwapBuffers();
	}

	return 0;
}