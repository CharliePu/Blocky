#pragma once
#include <gl\glew.h>
#include <GLFW\glfw3.h>

class Framebuffer
{
public:
	//constructor
	Framebuffer();
	
	//destructor
	//~Framebuffer();

	void use();

	//attachment
	void addTextureAttachment(const GLenum &type, const GLuint &width, const GLuint &height);
	void addRBOAttachment(const GLuint & width, const GLuint & height);

	//read-only variables
	const GLuint& texture();

	operator GLuint();
private:
	GLuint fbo, rbo;
	GLuint textureid;
};