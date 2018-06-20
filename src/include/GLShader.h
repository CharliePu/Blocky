#pragma once
#include <fstream>
#include <map>
#include <vector>
#include <string>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Debug.h"

//buffer the location of uniforms
typedef std::map<std::string, GLuint> UniformBuffer;

class GLShader
{
public:
	//constructor
	GLShader(const char* vertexFilePath, const char* fragmentFilePath);

	//destructor
	~GLShader();

	void use();
	friend GLuint getUniformLocation(const std::string& name);
	operator GLuint();
private:
	GLuint ID;
	UniformBuffer locationBuffer;


	//read-shader functions
	void readShaderCode(std::string & shaderCode, const std::string & filePath);
	void compileShader(const GLuint & shaderID, const std::string & filePath, const std::string & shaderCode);
	void linkShader(const GLuint & programID, const GLuint & vertexShaderID, const GLuint & fragmentShaderID);
};


void updateViewMatrix(const glm::vec3 &viewPosition, const glm::mat4 & viewMatrix);

void updateModelMatrix(const glm::mat4 & modelMatrix);

void updateModelMatrix(const glm::vec3 &position);

void updateProjectionMatrix(const double & fovy, const double & aspect, const double & near, const double &far);