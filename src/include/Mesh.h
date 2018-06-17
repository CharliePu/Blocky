#pragma once
#include <string>
#include <sstream>
#include <vector>

#include "Texture.h"
#include "Shader.h"

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

//index renderer
class Mesh 
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	//constructor
	Mesh();
	Mesh(const std::string &diffuseMap);
	Mesh(const std::string &diffuseMap, const std::string &specularMap);
	Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures);

	//destructor
//	~Mesh();


	void draw();

	//shape functions
	void addFlatRect(const glm::vec2 &upleft, const glm::vec2 &downleft, const glm::vec2 &downright, const glm::vec2 &upright, const glm::vec3 &normal);
	void addFlatRect(const glm::vec3 &upleft, const glm::vec3 &downleft, const glm::vec3 &downright, const glm::vec3 &upright, const glm::vec3 &normal);
	void addFlatRect(const std::vector<glm::vec3> &positions, const glm::vec3 & normal);
	void addCube(const GLfloat &width, const glm::vec3 &position);

private:
	GLuint VAO, VBO, EBO;
	void init();
	void update();
};


struct Vertex2D
{
	glm::vec2 position;
	glm::vec2 texCoords;
};

class Mesh2D
{
public:
	std::vector<Vertex2D> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	//constructor
	Mesh2D();
	Mesh2D(const GLuint& texture);

	//destructor
	//	~Mesh();


	void draw();
	
	//shape functions
	void addRect(const GLfloat &width, const GLfloat &height, const glm::vec2 &position);

private:
	GLuint VAO, VBO, EBO;
	void init();
	void update();
};
