#include "GLShader.h"

GLShader* currentShader;

GLShader::GLShader(const char* vertexFilePath, const char* fragmentFilePath)
{
	//create the shaders
	auto vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	auto fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//read the shader code
	std::string vertexShaderCode, fragmentShaderCode;
	readShaderCode(vertexShaderCode, vertexFilePath);
	readShaderCode(fragmentShaderCode, fragmentFilePath);

	//compile the shader
	compileShader(vertexShaderID, vertexFilePath, vertexShaderCode);
	compileShader(fragmentShaderID, fragmentFilePath, fragmentShaderCode);

	//link the shaders
	auto programID = glCreateProgram();
	linkShader(programID, vertexShaderID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	ID = programID;
}

GLShader::~GLShader()
{
	//delete shader program
	glDeleteProgram(this->ID);
}

void GLShader::use()
{
	//bind the shader
	glUseProgram(ID);

	//update current shader
	currentShader = this;
}

GLShader::operator GLuint()
{
	return ID;
}

void GLShader::readShaderCode(std::string &shaderCode, const std::string &filePath)
{

	std::ifstream shaderStream(filePath, std::ios::in);
	if (shaderStream.is_open())
	{
		std::string line = "";
		while (std::getline(shaderStream, line))
			shaderCode += "\n" + line;
	}
	else
	{
		sendError("Shader file " + filePath + " cannot be open");
	}
	
	shaderStream.close();
}

void GLShader::compileShader(const GLuint &shaderID, const std::string &filePath, const std::string &shaderCode)
{
	//compile the shader
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);

	//check the shader
	GLint result, infoLogLength;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<GLchar> shaderErrorMessage(infoLogLength + 1);//provide at least one space to prevent access violent
	glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]);
	if (shaderErrorMessage[0])
	{
		sendError("Compile shader source:" + (std::string)&shaderErrorMessage[0]);
	}
}

void GLShader::linkShader(const GLuint &programID, const GLuint &vertexShaderID, const GLuint &fragmentShaderID)
{
	//link the program
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	//check the program
	GLint result, infoLogLength;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<GLchar> programErrorMessage(infoLogLength > 1 ? infoLogLength : 1);
	glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
	if (programErrorMessage[0])
	{
		sendError("Linking shader program: " + (std::string)&programErrorMessage[0]);
	}
}

GLuint getUniformLocation(const std::string& name)
{
	static UniformBuffer::iterator i;
	i = currentShader->locationBuffer.find(name);
	if (i == currentShader->locationBuffer.end())
	{
		currentShader->locationBuffer[name] = glGetUniformLocation(*currentShader, name.c_str());
		return currentShader->locationBuffer[name];
	}
	else
	{
		return i->second;
	}
}

void updateViewMatrix(const glm::vec3 &viewPosition, const glm::mat4 & viewMatrix)
{
	glUniformMatrix4fv(getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3fv(getUniformLocation("viewPos"), 1, &viewPosition[0]);
}

void updateModelMatrix(const glm::mat4 & modelMatrix)
{
	glUniformMatrix4fv(getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void updateModelMatrix(const glm::vec3 & position)
{
	updateModelMatrix(glm::translate(glm::mat4(), position));
}

void updateProjectionMatrix(const double & fovy, const double & aspect, const double & near, const double &far)
{
	glUniformMatrix4fv(getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::perspective(fovy, aspect, near, far))));
}