#version 330							

// Input						
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;	// The direction perpendicular to the surface
			
// Output
out vec4 vCol;
out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vFragPos;

// Uniforms						
uniform mat4 model;						
uniform mat4 projection;
uniform mat4 view;

// Functions							
void main()							
{	
	gl_Position = projection * view * model * vec4(pos, 1.0f);	// Remember - the order is projection * view * model * identity matrix. Think of it as working backwards and moving through various spaces from there.
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);	

	vTexCoord = tex;
	vNormal = mat3(transpose(inverse(model))) * norm;	// The normal is in relation to where the model is
	vFragPos = (model * vec4(pos, 1.0f)).xyz;	// Use "swizzling" to grab the xyz.
}					