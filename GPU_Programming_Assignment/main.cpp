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

Texture brickTexture, dirtTexture, emeraldOreTexture, plainTexture;
Material shinyMaterial, dullMaterial;

Model xwing;
Model blackhawk;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

std::vector<Mesh*> meshList;
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
	unsigned int indices[] = {
		0, 3, 1,	// Left face
		1, 3, 2,	// Right face
		2, 3, 0,	// Front face
		0, 1, 2		// Bottom face
	};

	// Setup vertices of triangle - no depth, just three vertices within normalised space. Remember - the left side and bottom of our window are represented by -1.0 (or "100% left (or down)), or 1.0 for top or right of our window.
	GLfloat vertices[] = {
	//	x		y		z			u		v			nX		nY		nZ
		-1.0f,	-1.0f,	-0.6f,		0.0f,	0.0f,		0.0f,	0.0f,	0.0f,	// Bottom left 
		0.0f,	-1.0f,	1.0f,		0.5f,	0.0f,		0.0f,	0.0f,	0.0f,	// Background
		1.0f,	-1.0f,	-0.6f,		1.0f,	0.0f,		0.0f,	0.0f,	0.0f,	// Bottom right
		0.0f,	1.0f,	0.0f,		0.5f,	1.0f,		0.0f,	0.0f,	0.0f	// Top
	};

	CalculateAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

#pragma region Cube Exercise

	unsigned int cubeIndices[] = {
		0, 1, 2, 1, 3, 2,	// Front face
		1, 5, 3, 5, 7, 3,	// Right face	
		4, 5, 6, 5, 6, 7,	// Back face	
		4, 6, 0, 0, 2, 6,	// Left face
		2, 3, 6, 3, 7, 6,	// Top face
		0, 1, 4, 1, 5, 4	// Bottom face
	};

	GLfloat cubeVertices[] = {
	//	x		y		z			u		v			nX		nY		nZ		
		-1.0f,	-1.0f,	-1.0f,		0.0f,	0.0f,		0.0f,	0.0f,	0.0f,	// 0 - Front bottom left
		1.0f,	-1.0f,	-1.0f,		1.0f,	0.0f,		0.0f,	0.0f,	0.0f,	// 1 - Front bottom right
		-1.0f,	1.0f,	-1.0f,		0.0f,	1.0f,		0.0f,	0.0f,	0.0f,	// 2 - Front top left
		1.0f,	1.0f,	-1.0f,		1.0f,	1.0f,		0.0f,	0.0f,	0.0f,	// 3 - Front top right
		-1.0f,	-1.0f,	1.0f,		0.0f,	0.0f,		0.0f,	0.0f,	0.0f,	// 4 - Back bottom left
		1.0f,	-1.0f,	1.0f,		1.0f,	0.0f,		0.0f,	0.0f,	0.0f,	// 5 - Back bottom right
		-1.0f,	1.0f,	1.0f,		0.0f,	1.0f,		0.0f,	0.0f,	0.0f,	// 6 - Back top left
		1.0f,	1.0f,	1.0f,		1.0f,	1.0f,		0.0f,	0.0f,	0.0f	// 7 - Back top right
	};

	CalculateAverageNormals(cubeIndices, 36, cubeVertices, 64, 8, 5);
	Mesh* cube = new Mesh();
	cube->CreateMesh(cubeVertices, cubeIndices, 64, 36);
	meshList.push_back(cube);

#pragma endregion

#pragma region Floor Mesh
	unsigned int floorIndices[] = {
		0,	2,	1,
		1,	2,	3
	};

	GLfloat floorVertices[] = {
//	x		y		z		u		v		nX		nY		nZ
	-10.0f,	0.0f,	-10.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,
	10.0f,	0.0f,	-10.0f,	10.0f,	0.0f,	0.0f,	-1.0f,	0.0f,
	-10.0f,	0.0f,	10.0f,	0.0f,	10.0f,	0.0f,	-1.0f,	0.0f,
	10.0f,	0.0f,	10.0f,	10.0f,	10.0f,	0.0f,	-1.0f,	0.0f
	};

	Mesh* floor = new Mesh();
	floor->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(floor);
#pragma endregion

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

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	emeraldOreTexture = Texture("Textures/MC_Emerald_Ore.png");
	emeraldOreTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	shinyMaterial = Material(4.0f, 156);
	dullMaterial = Material(0.3f, 4);

	xwing = Model();
	xwing.LoadModel("Models/x-wing.obj");

	blackhawk = Model();
	blackhawk.LoadModel("Models/uh60.obj");

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
								0.3f, 0.6f, 
								0.0f, 0.0f, -1.0f);

	unsigned int pointLightCount = 0;
	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
								0.0f, 0.1f,
								0.0f, 0.0f, 0.0f,
								0.3f, 0.2f, 0.1f);
	pointLightCount++;
	
	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
								0.0f, 0.1f,
								-4.0f, 2.0f, 0.0f,
								0.3f, 0.1f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	spotLights[0] = SpotLight(	1.0f, 1.0f, 1.0f,
								0.0f, 2.0f,
								0.0f, 0.0f, 0.0f,
								0.0f, -1.0f, 0.0f,
								1.0f, 0.0f, 0.0f,
								20.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(	1.0f, 1.0f, 1.0f,
								0.0f, 1.0f,
								0.0f, -1.5f, 0.0f,
								-100.0f, -1.0f, 0.0f,
								1.0f, 0.0f, 0.0f,
								20.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;

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
		glClearColor(0.5f, 0.0f, 1.0f, 1.0f);	// glClear clears a screen, ready for us to draw to a new frame. glClearColor lets us set the colour of our new frame, not just a black void! Remember the colour values you set should be normalised.

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
		//spotLights[0].SetFlash(lowerLight, camera.GetCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);	// Note: the argument is a pointer, so we pass in the memory address
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// View and projection only need to be setup once. Model varies among different objects, so we will setup just view and projection once.
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.CalculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.GetCameraPosition().x, camera.GetCameraPosition().y, camera.GetCameraPosition().z);	// Inside our fragment shader we want to know the eye position, i.e., camera pos
		glm::mat4 model(1.0f); // Setup a 4x4 identity matrix so that we can calculate using it later

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));	// Take our identity matrix and apply a translation to it (as of writing this, just move in the x-axis)
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brickTexture.UseTexture();
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.UseTexture();
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, 0.0f, -2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		emeraldOreTexture.UseTexture();
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.UseTexture();
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-20.0f, 0.0f, 15.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		xwing.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-6.0f, 4.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		blackhawk.RenderModel();

		glUseProgram(0);	// Once we're done with a shader program, remember to unbind it.

		mainWindow.SwapBuffers();
	}

	return 0;
}