#include "Main.h"
GLFWwindow* window;
int windowWidth, windowHeight;
World aWorld;
GLCamera camera;

int main()
{
	initEnv();

	GLShader shader("..\\src\\shaders\\block.vsh", "..\\src\\shaders\\block.fsh"), debugShader("../src/shaders/debug.vsh", "../src/shaders/debug.fsh");
	

	aWorld.enableUpdateThread();

	while (!glfwWindowShouldClose(window))
	{
		updateEnv();
		camera.update(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		updateProjectionMatrix(45, windowWidth / (double)windowHeight, 0.1, 1000);
		updateViewMatrix(camera.position, camera.getViewMatrix());
		updateModelMatrix(glm::vec3(0, 0, 0));

		aWorld.draw();

		debugShader.use();
		updateProjectionMatrix(45, windowWidth / (double)windowHeight, 0.1, 1000);
		updateViewMatrix(camera.position, camera.getViewMatrix());
		updateModelMatrix(glm::vec3(0, 0, 0));

		aWorld.drawDebug();
	}

	aWorld.disableUpdateThread();
	glfwTerminate();
	return 0;
}

void initEnv()
{
	//init glfw library
	if (!glfwInit())
	{
		sendError("Failed to initialize GLFW");
	}

	//set opengl parameters
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	//create glfw window
	windowWidth = 500;
	windowHeight = 500;
	window = glfwCreateWindow(windowWidth, windowHeight, "EasyMinecraft", nullptr, nullptr);
	if (window == nullptr)
	{
		sendError("Failed to create GLFW window");
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	//init glew library
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		sendError("Failed to initialize GLEW");
	}

	glfwSwapInterval(0);
	//GL enable functions
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
//	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//init textures
	Texture::init("..\\src\\resource\\");
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
	glfwSetWindowTitle(window, ("Blocky[FPS:" + std::to_string(FPS) + "\tPosition:" + std::to_string((int)camera.position.x) + "," + std::to_string((int)camera.position.y) + "," + std::to_string((int)camera.position.z)+"]").c_str());
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
