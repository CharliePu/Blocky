#include <functional>
#include <gl\glew.h>

class GLFrameBuffer
{
public:
    enum Type
    {
		COLOR,
		DEPTH,
		STENCIL,
		DEPTH_AND_STENCIL
    };
	
	bool addTextureAttachment(const GLsizei &width, const GLsizei &height, const Type &type, const GLuint &i = 0)	
	{
		//get fbo id
		getFBO();
		glBindFramebuffer(GL_FRAMEBUFFER, id);

		//get texture id
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		//texture sampling setup
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//allocate texture and link to framebuffer
		switch (type)
		{
		case COLOR:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureId, 0);
			break;
		case DEPTH:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, 0);
			break;
		case STENCIL:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, width, height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureId, 0);
			break;
		case DEPTH_AND_STENCIL:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureId, 0);
			break;
		}
		//remove texture binding
		glBindTexture(GL_TEXTURE_2D, NULL);

		//test completion and remove framebuffer binding
		bool status = isComplete();
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
		return status;
	}

	bool addRenderObjectAttachment(const GLsizei &width, const GLsizei &height, const Type &type, const GLuint &i = 0)
	{
		//get fbo id
		getFBO();
		glBindFramebuffer(GL_FRAMEBUFFER, id);

		//get render buffer id
		GLuint rboId;
		glGenRenderbuffers(1, &rboId);
		glBindRenderbuffer(GL_RENDERBUFFER, rboId);

		//allocate render buffer and link to framebuffer
		switch (type)
		{
		case COLOR:
			glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, width, height);
			glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, rboId);
			break;
		case DEPTH:
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);
			break;
		case STENCIL:
			glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX, width, height);
			glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);
			break;
		case DEPTH_AND_STENCIL:
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);
			break;
		}
		//remove render buffer binding
		glBindRenderbuffer(GL_RENDERBUFFER, NULL);

		//test completion and remove framebuffer binding
		bool status = isComplete();
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
		return status;
	}
	
	void use(const std::function<void()>& func)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		func();
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	}

protected:
	bool isComplete()
	{
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}
	void getFBO()
	{
		if (id == 0)
			glGenFramebuffers(1, &id);
	}
	GLuint id;
};