#include "game/ball_manager.h"

#include "maths/basic.h"
#include "utils/log.h"

#include <fmt/format.h>

#include "game/game_manager.h"


namespace game
{
    BallManager::BallManager(core::EntityManager& entityManager, GameManager& gameManager, PhysicsManager& physicsManager, PlayerCharacterManager& playerCharacterManager):
	ComponentManager(entityManager), gameManager_(gameManager), physicsManager_(physicsManager), playerCharacterManager_(playerCharacterManager)
    {
    }


    void BallManager::FixedUpdate(sf::Time dt)
    {
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::BALL)))
            {
                continue;
            }
            const core::Vec2f maxPos{ (core::windowSize.x / core::pixelPerMeter / 2), (core::windowSize.y / core::pixelPerMeter / 2) };
            const core::Vec2f minPos{ -(core::windowSize.x / core::pixelPerMeter / 2), -(core::windowSize.y / core::pixelPerMeter / 2) };
            auto &ballBody = physicsManager_.GetCircle(entity);
            const auto player1 = gameManager_.GetEntityFromPlayerNumber(0);
            const auto player2 = gameManager_.GetEntityFromPlayerNumber(1);
            auto &playerCharacter1 = playerCharacterManager_.GetComponent(player1);
            auto &playerCharacter2 = playerCharacterManager_.GetComponent(player2);
        	if(core::Equal(ballBody.position.y, minPos.y + ballBody.radius) && ballBody.position.x > (maxPos.x+minPos.x)/2)
        	{
                core::LogWarning(fmt::format("Ball touched the ground on the right side!"));
                playerCharacter1.points += 1;
        	}
            if (core::Equal(ballBody.position.y, minPos.y + ballBody.radius) && ballBody.position.x < (maxPos.x+minPos.x)/2)
            {
                core::LogWarning(fmt::format("Ball touched the ground on the left side!"));
                playerCharacter2.points += 1;
            }
            playerCharacterManager_.SetComponent(entity, playerCharacter1);
            playerCharacterManager_.SetComponent(entity, playerCharacter2);
        }
    }
}
