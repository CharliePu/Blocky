#include <Shadow.h>
//


Shadow::Shadow()
{
	fb.addRenderObjectAttachment(1024, 1024, GLFrameBuffer::DEPTH);
	
	fb.use([]
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	});
}

void Shadow::prepare()
{
	glViewport(0, 0, 1024, 1024);
	fb.use([]
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	});
}
