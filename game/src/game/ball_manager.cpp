#include "game/ball_manager.h"


namespace game
{
    BallManager::BallManager(core::EntityManager& entityManager, GameManager& gameManager):
	ComponentManager(entityManager), gameManager_(gameManager)
    {
    }


    void BallManager::FixedUpdate(sf::Time dt)
    {
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::BALL)))
            {
            	
                auto& ball = components_[entity];
                
            }
        }
    }
}