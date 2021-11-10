#pragma once
#include <SFML/System/Time.hpp>

#include "game_globals.h"
#include "physics_manager.h"
#include "player_character.h"

namespace game
{
	struct Ball
	{
		PlayerNumber playerNumber = INVALID_PLAYER;
	};

	class GameManager;

	class BallManager : public core::ComponentManager<Ball, static_cast<core::EntityMask>(ComponentType::BALL)>
	{
	public:
		explicit BallManager(core::EntityManager& entityManager, GameManager& gameManager,
		                     PhysicsManager& physicsManager, PlayerCharacterManager& playerCharacterManager);
		void FixedUpdate(sf::Time dt) const;
	private:
		GameManager& gameManager_;
		PhysicsManager& physicsManager_;
		PlayerCharacterManager& playerCharacterManager_;
	};
}
