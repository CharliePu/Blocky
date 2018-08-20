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
			postPosition -= speed * deltaTime * glm::normalize(front * glm::vec3(1, 0, 1));
		if (glfwGetKey(window, GLFW_KEY_S))
			postPosition += speed * deltaTime * glm::normalize(front * glm::vec3(1, 0, 1));
		if (glfwGetKey(window, GLFW_KEY_A))
			postPosition += speed * deltaTime * glm::normalize(right * glm::vec3(1, 0, 1));
		if (glfwGetKey(window, GLFW_KEY_D))
			postPosition -= speed * deltaTime * glm::normalize(right * glm::vec3(1, 0, 1));
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
			postPosition -= speed * up * deltaTime;
		if (glfwGetKey(window, GLFW_KEY_SPACE))
		{
			postPosition += speed * up * deltaTime;
			if (fspeed == 0)
			{
				fspeed = 8;
				postPosition += fspeed * up * deltaTime;
			}
		}
		if (fspeed > -200)
		{
			fspeed -= 20 * deltaTime;
		}
		else
		{
			fspeed = -200;
		}
		postPosition.y += fspeed * deltaTime;

		if (position.y < -500)
			postPosition.y = 500;

		//if position inside chunk boundry 
		if (position.y < Chunk::sizeY && position.y > 0)
		{
			DirectionVec displace(postPosition - position);
			//if displace larger than one
			if (glm::length(displace) > 1) {
				float stepNum(glm::length(displace));
				PositionVec temp(position);
				DirectionVec unitStepVec(displace / stepNum);
				//test for collision by every block between starting and ending point to avoid skipping when speed is fast
				for (auto i = 0U; i < stepNum; ++i)
				{
					if (collide(temp, unitStepVec))
					{
						position = temp;
						fspeed = 0;
						return;
					}
					else
					{
						position = temp;
					}
				}
				//test collision for remaining displacement
				collide(temp, displace - std::floor(stepNum) * unitStepVec);
			}
			else
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

void Player::updateCursorCallback(GLFWwindow * window)
{
	GLCamera::updateCursorCallback(window);
	static bool lPressed = false;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
	{
		if (!lPressed)
		{
			lPressed = true;
			if (aWorld.setBlock(selectPos, Block::AIR))
			{
				aWorld.getCurrentChunk()->update();
			}
		}
	}
	else
	{
		lPressed = false;
	}

	static bool rPressed = false;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
	{
		if (!rPressed)
		{
			rPressed = true;
			if (selectBlock != Block::AIR)
			{
				if (putBlockNearFace(selectPos, selectFace, Block::COBBLESTONE))
				{
					aWorld.getCurrentChunk()->update();
				}
			}
		}
	}
	else
	{
		rPressed = false;
	}
}

//return state of collision
bool Player::collide(Player::PositionVec &pos, const Player::DirectionVec &displace)
{
	Player::PositionVec postPos(pos + displace);
	bool state = false;
	if (displace.y < 0)
	{
		if (aWorld.getBlock({ std::floor(pos.x),
			static_cast<Block::GlobalPosition>(std::floor(position.y + displace.y - height / 2)),
			std::floor(position.z) }))
		{
			postPos.y = std::floor(position.y + displace.y - height / 2) + 1 + height / 2;
			state = true;
		}
	}
	if (displace.x < 0)
	{
		if (aWorld.getBlock({ static_cast<Block::GlobalPosition>(std::floor(position.x + displace.x - width / 2)),
			std::floor(position.y),
			std::floor(position.z) }))
		{
			postPos.x = std::floor(position.x + displace.x - width / 2) + 1 + width / 2;
			state = true;
		}
	}
	if (displace.x > 0)
	{
		if (aWorld.getBlock({ static_cast<Block::GlobalPosition>(std::floor(position.x + displace.x + width / 2)),
			std::floor(position.y),
			std::floor(position.z) }))
		{
			postPos.x = std::floor(position.x + displace.x + width / 2) - width / 2;
			state = true;
		}
	}
	if (displace.z < 0)
	{
		//auto temp = static_cast<size_t>(std::floor(localPosZ + displace.z - length / 2)) + 1;
		if (aWorld.getBlock({ std::floor(position.x),
			std::floor(position.y),
			static_cast<Block::GlobalPosition>(std::floor(position.z + displace.z - length / 2)) }))
		{
			postPos.z = std::floor(position.z + displace.z - length / 2) + 1 + length / 2;
			state = true;
		}
	}
	if (displace.z > 0)
	{
		if (aWorld.getBlock({ std::floor(position.x),
			std::floor(position.y),
			static_cast<Block::GlobalPosition>(std::floor(position.z + displace.z + length / 2)) }))
		{
			postPos.z = std::floor(position.z + displace.z + length / 2) - length / 2;
			state = true;
		}
	}
	pos = postPos;
	return state;

}

void Player::selectUpdate()
{
	DirectionVec rayDir(glm::normalize(front));
	PositionVec checkPos(position + PositionVec(0, height / 2, 0));
	Block::GlobalPosVec truncPos;
	for (auto i = 0.0f; i < selectRadius; i += 0.01f)
	{
		checkPos -= rayDir * 0.01f;
		truncPos = { 
			static_cast<Block::GlobalPosition>(std::floor(checkPos.x)),
			static_cast<Block::GlobalPosition>(std::floor(checkPos.y)),
			static_cast<Block::GlobalPosition>(std::floor(checkPos.z)) 
		};
		if (aWorld.getBlock(truncPos))
		{
			break;
		}
	}

	selectBlock = aWorld.getBlock(truncPos);
	selectFace = getFace(checkPos);
	selectPos = truncPos;
}

void Player::selectDraw()
{
	std::vector<glm::fvec3> buffer{};
	for (auto i : Block::vertexLineIndices)
	{
		buffer.push_back(Block::vertexPositions[i] + glm::fvec3(selectPos) + glm::fvec3(0, -1, 0));
	}

	static GLuint vao = 0, vbo = 0;
	if (!vao || !vbo)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(glm::fvec3), &buffer[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::fvec3), NULL);
	if (aWorld.getBlock(selectPos))
		glDrawArrays(GL_LINES, 0, 48);
}

bool Player::putBlockNearFace(const Block::GlobalPosVec & pos, const Block::Face face, const Block::Type type)
{
	return aWorld.setBlock(pos + static_cast<Block::GlobalPosVec>(Block::vertexNormals[static_cast<size_t>(face)]), type);
}

Block::Face Player::getFace(const DirectionVec &point)
{
	float i;
	glm::vec3 internalPoint(
		std::modf(point.x, &i),
		std::modf(point.y, &i),
		std::modf(point.z, &i)
	);

	if (point.x < 0)
		internalPoint.x = 0 - internalPoint.x;
	if (point.y < 0)
		internalPoint.y = 0 - internalPoint.y;
	if (point.z < 0)
		internalPoint.z = 0 - internalPoint.z;

	float d(INFINITY), dis;
	Block::Face face;
	for (size_t i = 0; i != 6; ++i)
	{
		dis = glm::distance(internalPoint, Block::centerPointsOnFace[i]);
		if (dis < d)
		{
			d = dis;
			face = static_cast<Block::Face>(i);
		}
	}
	return face;
}