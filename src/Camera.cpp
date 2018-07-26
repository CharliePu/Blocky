#include "Camera.h"
extern World aWorld;

void Camera::updateKeyCallback(GLFWwindow * window)
{
	static float lastTime, currentTime;
	float deltaTime;
	lastTime = currentTime;
	currentTime = (float)glfwGetTime();
	deltaTime = currentTime - lastTime;

	Block::Position localPosX = static_cast<Block::Position>(std::floor(position.x + 0.5)) % Chunk::sizeX;
	Block::Position localPosZ = static_cast<Block::Position>(std::floor(position.z + 0.5)) % Chunk::sizeZ;
	Block::Position localPosY = static_cast<std::size_t>(std::ceil(position.y));

	glm::vec3 postPosition = position;
	Chunk *currentChunk(&aWorld.getCurrentChunk());
	
	if (glfwGetKey(window, GLFW_KEY_W))
		postPosition -= speed * deltaTime * glm::normalize(front * glm::vec3(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_S))
		postPosition += speed * deltaTime * glm::normalize(front * glm::vec3(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_A))
		postPosition += speed * deltaTime * glm::normalize(right * glm::vec3(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_D))
		postPosition -= speed * deltaTime * glm::normalize(right * glm::vec3(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_SPACE))
		postPosition += speed * up * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		postPosition -= speed * up * deltaTime;

	//if (position.y < Chunk::sizeY && position.y > 0)
	//{
	//	if (currentChunk->data[localPosX + 1][static_cast<std::size_t>(std::ceil(position.y - 1))][localPosZ + 1])
	//		postPosition.y = std::ceil(position.y - 1) + 1;
	//}
	//if (postPosition.x > 0)
	//{
	//	if (currentChunk->data[static_cast<std::size_t>(std::floor(position.x + 1)) + 1][localPosY][localPosZ + 1])
	//		postPosition.x = std::floor(position.x + 1) - 0.5;
	//}
	//if (postPosition.x < 0)
	//{
	//	if (currentChunk->data[static_cast<std::size_t>(std::ceil(position.x - 1)) + 1][localPosY][localPosZ + 1])
	//		postPosition.x = std::ceil(position.x - 1) + 0.5;
	//}
	//if (postPosition.z > 0)
	//{
	//	if (currentChunk->data[static_cast<std::size_t>(std::floor(position.z + 1)) + 1][localPosY][localPosZ + 1])
	//		postPosition.z = std::floor(position.z + 1) - 0.5;
	//}
	//if (postPosition.z < 0)
	//{
	//	if (currentChunk->data[static_cast<std::size_t>(std::ceil(position.z - 1)) + 1][localPosY][localPosZ + 1])
	//		postPosition.z = std::ceil(position.z - 1) + 0.5;
	//}
	position = postPosition;
}
