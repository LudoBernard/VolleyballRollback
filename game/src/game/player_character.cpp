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
		for (core::Entity playerEntity = 0; playerEntity < entityManager_.GetEntitiesSize(); playerEntity++)
		{
			if (!entityManager_.HasComponent(playerEntity,
			                                 static_cast<core::EntityMask>(ComponentType::PLAYER_CHARACTER)))
				continue;

			const core::Vec2f minPos{
				-(core::windowSize.x / core::pixelPerMeter / 2), -(core::windowSize.y / core::pixelPerMeter / 2)
			};
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


			physicsManager_.SetCircle(playerEntity, playerBody);
		}
	}
}
