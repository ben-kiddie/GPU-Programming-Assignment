#include "cShader.h"

Shader::Shader()
{
    shaderID = 0;
    uniformModel = 0;
    uniformProjection = 0;
	mPointLightCount = 0;
	mSpotLightCount = 0;
}

Shader::~Shader()
{
	ClearShader();
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
    CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	// Load our vertex and fragment shader code into strings
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);

	// Cast the strings to const char arrays. This is done by making use of std::string's .c_str() function.
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open())
	{
		printf("Failed to read %s! File doesn't exist.", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())	// Keep checking to see if we're at the end of the file. Each time .eof() is called, the program moves to the next line.
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint Shader::GetProjectionLocation()
{
	return uniformProjection;
}

GLuint Shader::GetModelLocation()
{
	return uniformModel;
}

GLuint Shader::GetViewLocation()
{
	return uniformView;
}

GLuint Shader::GetEyePositionLocation()
{
	return uniformEyePosition;
}

GLuint Shader::GetAmbientIntensityLocation()
{
	return uniformDirectionalLight.uniformAmbientIntensity;
}

GLuint Shader::GetAmbientColourLocation()
{
	return uniformDirectionalLight.uniformColour;
}

GLuint Shader::GetDiffuseIntensityLocation()
{
	return uniformDirectionalLight.uniformDiffuseIntensity;
}

GLuint Shader::GetDirectionLocation()
{
	return uniformDirectionalLight.uniformDirection;
}

GLuint Shader::GetSpecularIntensityLocation()
{
	return uniformSpecularIntensity;
}

GLuint Shader::GetShininessLocation()
{
	return uniformShininess;
}

void Shader::SetDirectionalLight(DirectionalLight* directionalLight)
{
	directionalLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity,		// Note: the '->' operation is because we're working with a pointer, as opposed to the usual '.function()'
								uniformDirectionalLight.uniformColour,
								uniformDirectionalLight.uniformDiffuseIntensity, 
								uniformDirectionalLight.uniformDirection);
}

void Shader::SetPointLights(PointLight* pointLights, unsigned int lightCount)
{
	if (lightCount > MAX_POINT_LIGHTS)	lightCount = MAX_POINT_LIGHTS;
	glUniform1i(uniformPointLightCount, lightCount);	// We use glUniform1i because we are now using integers while we loop through our lights
	
	// Go through each point light within the array we set a pointer to and grab the member variable uniform locations
	for (size_t i = 0; i < lightCount; i++){
		pointLights[i].UseLight(uniformPointLight[i].uniformAmbientIntensity, uniformPointLight[i].uniformColour,
			uniformPointLight[i].uniformDiffuseIntensity, uniformPointLight[i].uniformPosition,
			uniformPointLight[i].uniformConstant, uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent);
	}
}

void Shader::SetSpotLights(SpotLight* spotLights, unsigned int lightCount)
{
	if (lightCount > MAX_SPOT_LIGHTS)	lightCount = MAX_SPOT_LIGHTS;
	glUniform1i(uniformSpotLightCount, lightCount);	// We use glUniform1i because we are now using integers while we loop through our lights

	// Go through each point light within the array we set a pointer to and grab the member variable uniform locations
	for (size_t i = 0; i < lightCount; i++) {
		spotLights[i].UseLight(uniformSpotLight[i].uniformAmbientIntensity, uniformSpotLight[i].uniformColour,
			uniformSpotLight[i].uniformDiffuseIntensity, uniformSpotLight[i].uniformPosition, uniformSpotLight[i].uniformDirection,
			uniformSpotLight[i].uniformConstant, uniformSpotLight[i].uniformLinear, uniformSpotLight[i].uniformExponent,
			uniformSpotLight[i].uniformEdge);
	}
}

void Shader::UseShader()
{
	glUseProgram(shaderID);
}

void Shader::ClearShader()
{
	if (shaderID != 0)
	{
		glDeleteProgram(shaderID);
		shaderID = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	// Remember - shader is an ID. What we store in here is the results of glCreateProgram, which produces a program ID for us to bind our shaders to.
	shaderID = glCreateProgram();

	if (!shaderID)	// In the event no program ID was assigned to shader, we know there was en error
	{
		printf("Error creating shader program!\n");
		return;	// Exit to main early
	}

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	// Error checking our shader programs - the result of our program running is returned, and the error ID and log is stored for us to output.
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// Parameters of glLinkProgram:
	//	1 - The ID for our shader program we want to create exectuables for
	glLinkProgram(shaderID);

	// Parameters of glGetProgramiv:
	//	1 - The shader program ID
	//	2 - Which bit of information we want
	//	3 - Where we store the result of our check
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)	// If our result is erroneous, query the program log, store, and output it
	{
		// Parameters of glGetProgramInfoLog:
		//	1 - The shader program we are querying
		//	2 - The size of where we are storing the log
		//	3 - 
		//	4 - The place we store our log
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);

		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	// Parameters of glValidateProgram:
	//	1 - The shader program we are validating
	glValidateProgram(shaderID);	// Check to see if our shader program is valid in the context of what our OpenGL is working in

	// After validate program is called, we can query the validate status. Check if erroneous, and output log if so.
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	// Parameters of glGetUniformLocation:
	//	1 - The shader program we are using
	//	2 - The name of the uniform variable in our shader
	uniformModel = glGetUniformLocation(shaderID, "model");	// We're saying we want the ID (location) of the xMove uniform variable in our shader stored in the uniformXMove variable - reference this ID to modify inside the shader
	uniformProjection = glGetUniformLocation(shaderID, "projection");
	uniformView = glGetUniformLocation(shaderID, "view");
	uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");
	
	uniformDirectionalLight.uniformColour = glGetUniformLocation(shaderID, "directionalLight.base.colour");
	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");
	uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
	
	uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
	uniformShininess = glGetUniformLocation(shaderID, "material.shininess");
	
	uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");
	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		char locBuffer[100] = { '\0' };

		// Parameters of snprintf:
		//	1 - Pointer to a buffer where the resulting C-string is stored. The buffer should have a size of at least n characters.
		//	2 - Maximum number of bytes to be used in the buffer. The generated string has a length of at most n - 1, leaving space for the additional terminating null character. size_t is an unsigned integral type.
		//	3 - Format. C string that contains a format string that follows the same specifications as format in printf (see printf for details).
		//	4+ - Additional arguments. 
		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].base.colour", i);	// Print the contents of pointLights to locBuffer. Note: %d gets replaced with the suffixed 'i'passed in. The idea is that we have an array of point lights to access in our fragment shader.
		uniformPointLight[i].uniformColour = glGetUniformLocation(shaderID, locBuffer);	// For every point light, grab uniform location which we printed to our location buffer
	
		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].base.ambientIntensity", i);
		uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuffer);
	
		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].base.diffuseIntensity", i);
		uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuffer);
	
		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].position", i);
		uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuffer);
	
		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].constant", i);
		uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuffer);
	
		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].linear", i);
		uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuffer);
	
		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].exponent", i);
		uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuffer);
	}

	uniformSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");
	for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
	{
		char locBuffer[100] = { '\0' };

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.base.colour", i);
		uniformSpotLight[i].uniformColour = glGetUniformLocation(shaderID, locBuffer);	

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.base.ambientIntensity", i);
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.base.diffuseIntensity", i);
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.constant", i);
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.linear", i);
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.exponent", i);
		uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderID, locBuffer);
	}
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	// Create an empty shader of a given type - i.e., the type we pass in to this function
	GLuint theShader = glCreateShader(shaderType);

	// Create an array of pointers to our shader code
	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	// This is where the #include <string.h> comes in - we want to use strlen(), so that we can get the length of the actual code
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	// Parameters of glShaderSource:
	//	1 - The shader we modify the code for
	//	2 - Number of strings we are using in our shader program
	//	3 - The shader code
	//	4 - The shader code length
	glShaderSource(theShader, 1, theCode, codeLength);	// Setup the shader source code in memory to be modified to the code we want

	// Parameters of glCompileShader:
	//	1 - The source code of a shader in memory we wish to compile
	glCompileShader(theShader);

	// Error check the compiling of our shader source code
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		// Parameters of glGetShaderInfoLog:
		//	1 - The shader program we are querying
		//	2 - The size of memory we store our error log in
		//	3 - The type of shader we are querying
		//	4 - Where we store the error log
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	// Parameters of glAttachShader:
	//	1 - The compiled program we wish to attach our shader to
	//	2 - The type of shader we are attaching
	glAttachShader(theProgram, theShader);
}
