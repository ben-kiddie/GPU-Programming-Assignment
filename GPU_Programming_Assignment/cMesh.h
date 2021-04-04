#pragma once

#include<GL/glew.h>

class Mesh
{
public:
	Mesh();
	~Mesh();

	/// <summary>
	/// Setup VAOs, VBOs and IBOs for which we will draw from.
	/// </summary>
	/// <param name="vertices">Pointer to an array of vertices which the mesh is constructed from</param>
	/// <param name="indices">Pointer to an array of indices which correspond to elements in our vertices array</param>
	/// <param name="numOfVertices">Number of elements in our vertices array</param>
	/// <param name="numOfIndices">Number of elements in our indices array</param>
	void CreateMesh(GLfloat *vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices);	
	
	/// <summary>
	/// Binds, draws, and unbinds the relevant object buffers and arrays.
	/// </summary>
	void RenderMesh();
	
	/// <summary>
	/// Clear all the buffers and arrays on our graphics card related to this object.
	/// </summary>
	void ClearMesh();

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

