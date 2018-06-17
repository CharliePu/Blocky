#include "Mesh.h"


Mesh::Mesh()
{
	this->init();
}

Mesh::Mesh(const std::string & diffuseMap)
{
	this->textures.push_back(Texture(diffuseMap, texture_diffuse_map));
	this->init();
}

Mesh::Mesh(const std::string & diffuseMap, const std::string & specularMap)
{
	this->textures.push_back(Texture(diffuseMap, texture_diffuse_map));
	this->textures.push_back(Texture(specularMap, texture_specular_map));
	this->init();
}

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures)
{
	//import data
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	//create buffers and upload data
	this->init();
	this->update();
}

//Mesh::~Mesh()
//{
//	glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &EBO);
//	glDeleteBuffers(GL_ARRAY_BUFFER, &VBO);
//
//	glDeleteVertexArrays(1, &VAO);
//}

void Mesh::draw()
{
	//classify cube map or 2D map(diffuse, specular)
	if (this->textures.size() > 0)
		if (this->textures[0] == GL_TEXTURE_CUBE_MAP)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, this->textures[0]);
		}
		else
		{
			GLuint diffuseNr = 1;
			GLuint specularNr = 1;
			GLuint reflectionNr = 1;
			for (GLuint i = 0; i < this->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				std::stringstream ss;
				std::string number;
				std::string name;
				if (this->textures[i].type == texture_diffuse_map)
				{
					name = "texture_diffuse";
					ss << diffuseNr++;
				}
				else if (this->textures[i].type == texture_specular_map)
				{
					name = "texture_specular";
					ss << specularNr++;
				}
				else if (this->textures[i].type == texture_reflection_map)
				{
					name = "texture_reflection";
					ss << reflectionNr++;
				}

				number = ss.str();
				glUniform1f(getUniformLocation((name + number).c_str()), (GLfloat)i);
				glBindTexture(GL_TEXTURE_2D, this->textures[i]);
			}
			glActiveTexture(GL_TEXTURE0);
		}

	std::cout << this->indices.size() <<std::endl;
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::init()
{
	// create buffers/arrays
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
}

void Mesh::update()
{
	glBindVertexArray(this->VAO);
	//upload vertices
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	//upload indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	// texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
	glBindVertexArray(0);
}

void Mesh::addFlatRect(const glm::vec2 & upleft, const glm::vec2 & downleft, const glm::vec2 & downright, const glm::vec2 & upright, const glm::vec3 & normal)
{
	addFlatRect(std::vector<glm::vec3>{glm::vec3(upleft, 0), glm::vec3(downleft, 0), glm::vec3(downright, 0), glm::vec3(upright, 0)}, normal);
}

void Mesh::addFlatRect(const glm::vec3 & upleft, const glm::vec3 & downleft, const glm::vec3 & downright, const glm::vec3 & upright, const glm::vec3 & normal)
{
	addFlatRect(std::vector<glm::vec3>{upleft, downleft, downright, upright}, normal);
}

void Mesh::addFlatRect(const std::vector<glm::vec3> &positions, const glm::vec3 & normal)
{
	static const glm::vec2 texCoords[4] = { glm::vec2(0,0),glm::vec2(0,1),glm::vec2(1,1), glm::vec2(1,0) };
	static const GLuint indices[6] = { 0,1,2,2,3,0 };

	//import vertices
	for (unsigned int i = 0; i < positions.size(); i++)
	{
		this->vertices.push_back(Vertex{ positions[i], normal, texCoords[i % 4] });
	}
	//import indices
	for (unsigned int i = 0; i < 6; i++)
	{
		this->indices.push_back(indices[i]);
	}
	
	update();
}

void Mesh::addCube(const GLfloat & width, const glm::vec3 & position)
{
	std::vector<glm::vec3> vertices =
	{
		glm::vec3(-width/2, width/2, width/2) + position,
		glm::vec3(-width/2,-width/2, width/2) + position,
		glm::vec3( width/2,-width/2, width/2) + position,
		glm::vec3( width/2, width/2, width/2) + position,
		glm::vec3(-width/2, width/2,-width/2) + position,
		glm::vec3(-width/2,-width/2,-width/2) + position,
		glm::vec3( width/2,-width/2,-width/2) + position,
		glm::vec3( width/2, width/2,-width/2) + position
	};
	static std::vector<GLuint> indices =//every face with two triangles
	{
		//right
		3,2,6,
		6,7,3,
		//left
		4,5,1,
		1,0,4,
		//up
		4,0,3,
		3,7,4,
		//down
		1,5,6,
		6,2,1,
		//front
		0,1,2,
		2,3,0,
		//back
		7,6,5,
		5,4,7
	};
	static std::vector<glm::vec3> normal =
	{
		glm::vec3( 1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3( 0, 1, 0),
		glm::vec3( 0,-1, 0),
		glm::vec3( 0, 0, 1),
		glm::vec3( 0, 0,-1)
	};
	static const glm::vec2 texCoords[6] = 
	{ 
		//y-value has been inversed for adapting openGL texcord system
		glm::vec2(0,0),
		glm::vec2(0,1),
		glm::vec2(1,1),
		glm::vec2(1,1), 
		glm::vec2(1,0),
		glm::vec2(0,0)
	};
	Vertex temp;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			temp.position = vertices[indices[i * 6 + j]];
			temp.normal = normal[i];
			temp.texCoords = texCoords[j];
			this->vertices.push_back(temp);
			this->indices.push_back(i * 6 + j);
		}
	}

	this->update();
}

Mesh2D::Mesh2D()
{
	this->init();
}

Mesh2D::Mesh2D(const GLuint& texture)
{
	this->textures.push_back(Texture(texture, texture_diffuse_map));
	this->init();
}

void Mesh2D::draw()
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		std::stringstream ss;
		std::string number;
		std::string name;
		if (this->textures[i].type == texture_diffuse_map)
		{
			name = "texture_diffuse";
			ss << diffuseNr++;
		}
		else if (this->textures[i].type == texture_specular_map)
		{
			name = "texture_specular";
			ss << specularNr++;
		}
		number = ss.str();
		glUniform1f(getUniformLocation((name + number).c_str()), (GLfloat)i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i]);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh2D::addRect(const GLfloat & width, const GLfloat & height, const glm::vec2 & position)
{
	//import vertices
	std::vector<Vertex2D> vertices = {   
		Vertex2D{ position + glm::vec2(-width / 2, height / 2), glm::vec2(0,1)},
		Vertex2D{ position + glm::vec2(-width / 2,-height / 2), glm::vec2(0,0)},
		Vertex2D{ position + glm::vec2( width / 2,-height / 2), glm::vec2(1,0)},
		Vertex2D{ position + glm::vec2( width / 2, height / 2), glm::vec2(1,1)}
	};
	this->vertices.insert(this->vertices.end(), vertices.begin(), vertices.end());

	//import indices
	std::vector<GLuint> indices = { 0,1,2,2,3,0 };
	this->indices.insert(this->indices.end(), indices.begin(), indices.end());

	this->update();
}

void Mesh2D::init()
{
	// create buffers/arrays
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
}

void Mesh2D::update()
{
	glBindVertexArray(this->VAO);
	//upload vertices
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex2D), &this->vertices[0], GL_STATIC_DRAW);

	//upload indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)0);
	// texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, texCoords));
	glBindVertexArray(0);
}
