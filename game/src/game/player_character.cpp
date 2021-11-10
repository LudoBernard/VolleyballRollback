#include <game/player_character.h>
#include <game/game_manager.h>

#include "maths/basic.h"

namespace game
{
	PlayerCharacterManager::PlayerCharacterManager(core::EntityManager& entityManager, PhysicsManager& physicsManager,
	                                               GameManager& gameManager) :
		ComponentManager(entityManager),
		physicsManager_(physicsManager),
		gameManager_(gameManager)

	{
	}

	void PlayerCharacterManager::FixedUpdate(sf::Time dt)
	{
		for (PlayerNumber playerNumber = 0; playerNumber < maxPlayerNmb; playerNumber++)
		{
			const auto playerEntity = gameManager_.GetEntityFromPlayerNumber(playerNumber);
		
			const core::Vec2f minPos = core::Vec2f::zero() - gameAreaSize / 2;
			auto playerBody = physicsManager_.GetCircle(playerEntity);
			const auto playerCharacter = GetComponent(playerEntity);
			const auto input = playerCharacter.input;
			const auto velocityMax = 2.5f;

			const bool right = input & PlayerInputEnum::PlayerInput::RIGHT;
			const bool left = input & PlayerInputEnum::PlayerInput::LEFT;
			const bool jump = input & PlayerInputEnum::PlayerInput::JUMP;


			sf::Vector2<float> dir = {playerSpeed, 0.0f};


			if (jump && core::Equal(playerBody.position.y, minPos.y + playerBody.radius))
			{
				playerBody.velocity.y += playerSpeed;
			}

			const auto newVelocity = ((left ? -1.0f : 0.0f) + (right ? 1.0f : 0.0f)) * dir;
			playerBody.velocity.x = newVelocity.x;
			if (playerBody.velocity.x >= velocityMax)
			{
				playerBody.velocity.x = velocityMax;
			}
			if (playerBody.velocity.x <= -velocityMax)
			{
				playerBody.velocity.x = -velocityMax;
			}
			
			if(playerBody.position.x - playerBody.radius < 0 && playerBody.velocity.x < 0 && playerNumber == 1)
			{
				playerBody.velocity.x = 0;
			}

			else if (playerBody.position.x + playerBody.radius > 0 && playerBody.velocity.x > 0 && playerNumber == 0)
			{
				playerBody.velocity.x = 0;
			}

			physicsManager_.SetCircle(playerEntity, playerBody);
		}
	}
}
