#include "Main.h"
GLFWwindow* window;
int windowWidth, windowHeight;
World aWorld;
Player camera;
GLLighting::DirectionLight sun;


int main()
{
	initEnv();
	
	GLShader shader("..\\src\\shaders\\block.vsh", "..\\src\\shaders\\block.fsh"), debugShader("../src/shaders/debug.vsh", "../src/shaders/debug.fsh");
	sun.set({ 0,-1,0 }, { 0.7,0.7,0.7 }, { 0.7,0.7,0.7 }, { 0.2,0.2,0.2 });

	aWorld.enableUpdateThread();


	while (!glfwWindowShouldClose(window))
	{
		updateEnv();
		camera.update(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		updateProjectionMatrix(45, windowWidth / (double)windowHeight, 0.1, 100000);
		updateViewMatrix(camera.getPosition(), camera.getViewMatrix());
		updateModelMatrix(glm::vec3(0, 0, 0));
		glm::vec3 viewDir = -glm::normalize(camera.getFront());
		glUniform3fv(getUniformLocation("cameraDir"), 1, glm::value_ptr(viewDir));
		glm::vec3 viewPos = camera.getPosition();
		glUniform3fv(getUniformLocation("cameraPos"), 1, glm::value_ptr(viewPos));
		aWorld.updateProceduralFog();
		sun.apply();
		aWorld.draw();
		
		camera.selectUpdate();

		debugShader.use();
		updateProjectionMatrix(45, windowWidth / (double)windowHeight, 0.1, 100000);
		updateViewMatrix(camera.getPosition(), camera.getViewMatrix());
		updateModelMatrix(glm::vec3(0, 0, 0));

		camera.selectDraw();

#ifdef _DEBUG
		aWorld.drawDebug();
#endif
		aWorld.updateCurrentChunkPosition();
		aWorld.unloadDistantChunks();
	}

	aWorld.disableUpdateThread();
	aWorld.removeAll();

	glfwTerminate();
	return 0;
}

void initEnv()
{
	//init glfw library
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
	}

	//set opengl parameters
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	//create glfw window
	windowWidth = 2000;
	windowHeight = 1500;
	window = glfwCreateWindow(windowWidth, windowHeight, "EasyMinecraft", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	//init glew library
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
	}

	glfwSwapInterval(1);
	//GL enable functions
//	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3.0f);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Gamma correction
	//glEnable(GL_FRAMEBUFFER_SRGB);

//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//	glDepthFunc(GL_LEQUAL);
	glClearColor(0.5f, 0.8f, 1.0f, 0.0f);

	//init textures
	Texture::init("..\\src\\resource\\src\\");
}

void updateEnv()
{
	//exchange the buffer
	glfwSwapBuffers(window);
	
	//update window properties
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glfwPollEvents();
	
	//key detection
	updateWindowKeyCallback();

	static int frameRate = 0, FPS;
	static double lastTime = 0;
	frameRate++;
	if (glfwGetTime() - lastTime > 1)
	{
		FPS = frameRate;
		frameRate = 0;
		lastTime = glfwGetTime();
	}
	int chunkX = 0, chunkZ = 0;
	if (aWorld.getCurrentChunk())
	{
		chunkX = aWorld.getCurrentChunk()->chunkX;
		chunkZ = aWorld.getCurrentChunk()->chunkZ;
	}
	glfwSetWindowTitle(window, ("Blocky FPS:" + std::to_string(FPS) + 
		"CurrentChunk[" + std::to_string(chunkX) + ", " + std::to_string(chunkZ) + 
		"] Camera[" + std::to_string(camera.getPosition().x) +
		", " + std::to_string(camera.getPosition().y) +
		", " + std::to_string(camera.getPosition().z)).c_str());
}

void updateWindowKeyCallback()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, GL_TRUE);
	static bool b;
	if (glfwGetKey(window, GLFW_KEY_P))
		if (!b)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			b = !b;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			b = !b;
		}
}
