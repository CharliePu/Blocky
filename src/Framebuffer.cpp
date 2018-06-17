#include "Framebuffer.h"

Framebuffer::Framebuffer()
{
	//generate and bind framebuffer
	glGenFramebuffers(1, &this->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

//Framebuffer::~Framebuffer()
//{
//	//delete framebuffer
//	glDeleteFramebuffers(GL_FRAMEBUFFER, &fbo);
//}

void Framebuffer::use()
{
	glBindBuffer(GL_FRAMEBUFFER, this->fbo);
}

void Framebuffer::addTextureAttachment(const GLenum & type, const GLuint & width, const GLuint & height)
{
	//generate and bind texture
	glGenTextures(1, &this->textureid);
	glBindTexture(GL_TEXTURE_2D, this->textureid);

	//allocate memory without import data
	switch (type)
	{
	case GL_COLOR_ATTACHMENT0:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		break;
	case GL_DEPTH_ATTACHMENT:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		break;
	case GL_STENCIL_ATTACHMENT:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, width, height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, nullptr);
		break;
	case GL_DEPTH_STENCIL_ATTACHMENT:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		break;
	}

	//set texture's parameter(no mipmap or clamp)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, NULL);

	//attach the texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D, this->textureid, 0);
}

void Framebuffer::addRBOAttachment(const GLuint & width, const GLuint & height)
{
	//generate and bind renderbuffer
	glGenRenderbuffers(1, &this->rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	//allocate memoery
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	
	//attach renderbuffer to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

const GLuint & Framebuffer::texture()
{
	return textureid;
}

Framebuffer::operator GLuint()
{
	return fbo;
}
