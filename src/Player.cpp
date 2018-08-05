#include "Player.h"
extern World aWorld;

void Player::updateKeyCallback(GLFWwindow * window)
{
	// refresh deltatime
	static float lastTime, currentTime;
	float deltaTime;
	lastTime = currentTime;
	currentTime = (float)glfwGetTime();
	deltaTime = currentTime - lastTime;


	glm::vec3 postPosition = position;
	Chunk *currentChunk(aWorld.getCurrentChunk());

	static float fspeed = 0;
	if (currentChunk)
	{
		//Keyboardcallback
		if (glfwGetKey(window, GLFW_KEY_W))
			postPosition -= speed * deltaTime * front;
		if (glfwGetKey(window, GLFW_KEY_S))
			postPosition += speed * deltaTime * front;
		if (glfwGetKey(window, GLFW_KEY_A))
			postPosition += speed * deltaTime * right;
		if (glfwGetKey(window, GLFW_KEY_D))
			postPosition -= speed * deltaTime * right;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
			postPosition -= speed * up * deltaTime;
		if (glfwGetKey(window, GLFW_KEY_SPACE))
		{
			if (fspeed == 0)
			{
				fspeed = 8;
				postPosition += fspeed * up * deltaTime;
			}
		}

		fspeed -= 20 * deltaTime;
		postPosition.y += fspeed * deltaTime;

		if (position.y < -500)
			postPosition.y = 500;

		//if position inside chunk boundry 
		if (position.y < Chunk::sizeY && position.y > 0)
		{
			DirectionVec displace(postPosition - position);
			//if displace larger than one
			//if (glm::length(displace) > 1) {
			//	float stepNum(glm::length(displace));
			//	PositionVec temp(position);
			//	DirectionVec unitStepVec(displace / stepNum);

			//	//test for collision by every block between starting and ending point to avoid skipping when speed is fast
			//	for (auto i = 0U; i < stepNum; ++i)
			//	{
			//		if (collide(temp, unitStepVec))
			//		{
			//			position = temp;
			//			return;
			//		}
			//		else
			//		{
			//			position = temp;
			//		}
			//	}
			//	//test collision for remaining displacement
			//	collide(temp, displace - std::floor(stepNum) * unitStepVec);
			//}
			//else
			{
				//if displacement is smaller than 1, it is safe to test collision directly at the ending point
				//collide(position, displace);
				if (collide(position, displace))
				{
					fspeed = 0;
				}
			}
		}
		else
		{
			//if position is too high or too low, do no collision check
			position = postPosition;
		}
	}
}

//return state of collision, will change pos
bool Player::collide(Player::PositionVec &pos, const Player::DirectionVec &displace)
{
	Player::PositionVec postPos(pos + displace);
	bool state = false;
	try
	{
		if (displace.y < 0)
		{
			//if (currentChunk->data[truncLocalPosX + 1][static_cast<size_t>(std::floor(localPosY + displace.y - height / 2))][truncLocalPosZ + 1])
			if (aWorld.getBlock(std::floor(pos.x),
				static_cast<Block::GlobalPosition>(std::floor(position.y + displace.y - height / 2)),
				std::floor(position.z)))
			{
				postPos.y = std::floor(position.y + displace.y - height / 2) + 1 + height / 2;
				state = true;
			}
		}
		if (displace.x < 0)
		{
			//if (currentChunk->data[static_cast<size_t>(std::floor(localPosX + displace.x - width / 2)) + 1][truncLocalPosY][truncLocalPosZ + 1])
			if (aWorld.getBlock(static_cast<Block::GlobalPosition>(std::floor(position.x + displace.x - width / 2)),
				std::floor(position.y),
				std::floor(position.z)))
			{
				postPos.x = std::floor(position.x + displace.x - width / 2) + 1 + width / 2;
				state = true;
			}
		}
		if (displace.x > 0)
		{
			//if (currentChunk->data[static_cast<size_t>(std::floor(localPosX + displace.x + width / 2)) + 1][truncLocalPosY][truncLocalPosZ + 1])
			if (aWorld.getBlock(static_cast<Block::GlobalPosition>(std::floor(position.x + displace.x + width / 2)),
				std::floor(position.y),
				std::floor(position.z)))
			{
				postPos.x = std::floor(position.x + displace.x + width / 2) - width / 2;
				state = true;
			}
		}
		if (displace.z < 0)
		{
			//auto temp = static_cast<size_t>(std::floor(localPosZ + displace.z - length / 2)) + 1;
			//if (currentChunk->data[truncLocalPosX + 1][truncLocalPosY][temp >= 0 ? static_cast<Block::Position>(temp) : 0])
			if (aWorld.getBlock(std::floor(position.x),
				std::floor(position.y),
				static_cast<Block::GlobalPosition>(std::floor(position.z + displace.z - length / 2))))
			{
				postPos.z = std::floor(position.z + displace.z - length / 2) + 1 + length / 2;
				state = true;
			}
		}
		if (displace.z > 0)
		{
			//if (currentChunk->data[truncLocalPosX + 1][truncLocalPosY][static_cast<size_t>(std::floor(localPosZ + displace.z + length / 2)) + 1])
			if (aWorld.getBlock(std::floor(position.x),
				std::floor(position.y),
				static_cast<Block::GlobalPosition>(std::floor(position.z + displace.z + length / 2))))
			{
				postPos.z = std::floor(position.z + displace.z + length / 2) - length / 2;
				state = true;
			}
		}
		pos = postPos;
		return state;
	}
	catch (std::exception &e)
	{
		return true;
	}
}