#include "cMesh.h"

Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

Mesh::~Mesh()
{
	ClearMesh();	// If the object gets lost and deleted somewhere, automatically clear the GPU buffer memory
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount = numOfIndices;

	// Parameters of glGenVertexArrays:
	//	1 - The number of arrays we want to generate
	//	2 - The ID for which we want our new vertex array to correspond to
	glGenVertexArrays(1, &VAO);	// Generate a space in memory for a single vertex array with a given VAO as an ID - remember, VAO is simply a GLuint, or GL unsigned int!

	// Parameters of glBindVertexArray:
	//	1 - The ID for which vertex array to bind
	glBindVertexArray(VAO);	// For now, we just want to bind a single VAO for our triangle

	// Setup our Index Buffer Object, bind it, then generate the buffer data for it
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);	// Specify element array buffer - i.e., storing elements or "indices"
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	// Parameters of glGenBuffers:
	//	1 - The number of buffers we wish to create for a VAO
	//	2 - The ID for which VBO to assign to
	glGenBuffers(1, &VBO);	// Works basically the same as glGenVertexArrays, except we do it for every VBO we want per VAO
	
	// Parameters of glBindBuffer:
	//	1 - The target buffer we want to join to
	//	2 - The ID for which buffer we want to bind to
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Parameters of glBufferData:
	//	1 - The target buffer we are working with
	//	2 - The memory size of our values being passed in
	//	3 - The actual array of data we want to pass in
	//	4 - Whether the data you pass in will not be modified (GL_STATIC_DRAW), or is able to be modified (GL_DYNAMIC_DRAW)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);	// For the sizeof part, you could instead just take the sizeof the data type of your array, multiplied by the number of elements in your array

	// Parameters of glVertexAttribPointer:
	//	1 - The location of the attribute we are making the pointer for, e.g., in our shader we have a variable in location 0 - this is where we point to
	//	2 - The size of values being passed in
	//	3 - The type of values passed in
	//	4 - Enable normalising values (GL_TRUE), or do not normalise (GL_FALSE)
	//	5 - The stride of our data, e.g., we may want to include colour following our positions in an array - colour has 4 floats, position has 3, so our stride would be 7 per vertex (vertex being the accumulation of both position and colour)
	//	6 - The vertex offset, so we could stride into our array before reading the data, e.g., stride in two elements and begin from element 2
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);	// Here we simply say that, the variable at location 0 in our shader, we store an array where each vertex has 3 components, are of GL_FLOAT type, and have no stride, offset, or normalisation

	// Parameters of glEnableVertexAttribArray:
	//	1 - The location in our shader layout we are enabling
	glEnableVertexAttribArray(0);	// At location 0 in our shader, we are enabling it for us to pass vertex arrays to

	// Set the vertex attrib pointer for slot 1, the texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));	// Offset by three floats, as starting from zero we get positions, 3 floats in is where tex coords are
	glEnableVertexAttribArray(1);

	// Setup vertex normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
	glEnableVertexAttribArray(2);

	// Unbind the VBO, then unbind the VAO, followed by IBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);	// Once we've bound a shader, we decide which VAO we want to use, and pass int he ID
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Parameters of glDrawElements:
	//	1 - The primitive we are using, e.g., GL_TRIANGLES
	//	2 - The total number of elements in our indices array
	//	3 - The format of each index
	//	4 - A pointer to our indices (note: if you bind the index buffer just beforehand, no need to pass a pointer, send in 0)
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);	// After finishing with a VAO, unbind it
}

void Mesh::ClearMesh()
{
	if (IBO != 0)	// There is no garbage collection for memory we store on the graphics card, so we manually clear the memory
	{
		//Parameters of glDeleteBuffers:
		//	1 - Number of buffers to be deleted
		//	2 - Reference to the buffer(s) we are deleting
		glDeleteBuffers(1, &IBO);	// Delete the buffer from graphics card memory
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}