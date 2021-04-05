#version 330 core

// Input						
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;	// The direction perpendicular to the surface
			
// Output
out vec2 vTexCoord;

void main()
{
	vTexCoord = tex;

	gl_Position = vec4(pos.x, pos.y, 0.0f, 1.0f);
}