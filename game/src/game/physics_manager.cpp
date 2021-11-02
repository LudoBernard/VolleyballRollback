#include <game/physics_manager.h>

namespace game
{

    PhysicsManager::PhysicsManager(core::EntityManager& entityManager) :
        bodyManager_(entityManager), boxManager_(entityManager), entityManager_(entityManager),
        circleManager_(entityManager)
    {

    }

    bool Box2Box(float r1x, float r1y, float r1w, float r1h, float r2x, float r2y, float r2w, float r2h)
    {
        return r1x + r1w >= r2x &&    // r1 right edge past r2 left
            r1x <= r2x + r2w &&    // r1 left edge past r2 right
            r1y + r1h >= r2y &&    // r1 top edge past r2 bottom
            r1y <= r2y + r2h;
    }

    
	
    void PhysicsManager::ResolveBodyIntersect(CircleBody& body1, CircleBody& body2)
    {
        float v1n = ComputeNormal(body1.position, ContactPoint(body1, body2)).x * body1.velocity.x +
	        ComputeNormal(body1.position, ContactPoint(body1, body2)).y * body1.velocity.y;
        float v1t = ComputeTangent(body1.position, ContactPoint(body1, body2)).x * body1.velocity.x +
            ComputeTangent(body1.position, ContactPoint(body1, body2)).y * body1.velocity.y;
        float v2n = ComputeNormal(body2.position, ContactPoint(body1, body2)).x * body2.velocity.x +
            ComputeNormal(body2.position, ContactPoint(body1, body2)).y * body2.velocity.y;
        float v2t = ComputeTangent(body2.position, ContactPoint(body1, body2)).x * body2.velocity.x +
            ComputeTangent(body2.position, ContactPoint(body1, body2)).y * body2.velocity.y;

        body1.velocity.x = ComputeNormal(body1.position, ContactPoint(body1, body2)).x * v2n + ComputeTangent(
            body1.position, ContactPoint(body1, body2)).x * v1t * -body1.bounciness;
        body1.velocity.y = ComputeNormal(body1.position, ContactPoint(body1, body2)).y * v2n + ComputeTangent(
            body1.position, ContactPoint(body1, body2)).y * v1t * -body1.bounciness;
        body2.velocity.x = ComputeNormal(body2.position, ContactPoint(body1, body2)).x * v1n + ComputeTangent(
            body2.position, ContactPoint(body1, body2)).x * v2t * -body2.bounciness;
        body2.velocity.y = ComputeNormal(body2.position, ContactPoint(body1, body2)).y * v1n + ComputeTangent(
            body2.position, ContactPoint(body1, body2)).y * v2t * -body2.bounciness;

        body1.position = RelocateCenter(body1, ContactPoint(body1, body2));
        body2.position = RelocateCenter(body2, ContactPoint(body1, body2));
        body1.velocity = body1.velocity * -body1.bounciness;
        body2.velocity = body2.velocity * -body2.bounciness;
    }

    core::Vec2f PhysicsManager::ContactPoint(const CircleBody& body1, const CircleBody& body2) const
    {
        double ratio = (body1.radius) / ((body1.radius)+(body2.radius));
        return (body2.position - body1.position) * ratio + body1.position;
    }

    core::Vec2f PhysicsManager::RelocateCenter(const CircleBody& body, const core::Vec2f& v)
    {
        double ratio = (body.radius) / (body.position - v).Length();
        return (body.position - v) * ratio + v;
    }

    float PhysicsManager::CalculateDistance(CircleBody body1, CircleBody body2)
    {
        const float dx = body2.position.x - body1.position.x;
        const float dy = body2.position.y - body1.position.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    bool PhysicsManager::BodyIntersect(CircleBody body1, CircleBody body2)
    {
        return CalculateDistance(body1, body2) < (body1.radius + body2.radius);
    }
	
    void PhysicsManager::FixedUpdate(sf::Time dt)
    {
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(core::ComponentType::BODY2D)))
                continue;
            core::Vec2f G = { 0.0f, -9.81f };
            core::Vec2f maxPos{(core::windowSize.x / core::pixelPerMeter/2), (core::windowSize.y / core::pixelPerMeter/2) };
            core::Vec2f minPos{ -(core::windowSize.x / core::pixelPerMeter/2), -(core::windowSize.y / core::pixelPerMeter/2) };
            auto body = bodyManager_.GetComponent(entity);
            body.position += body.velocity * dt.asSeconds();
            body.velocity += G * dt.asSeconds();
        	if(body.position.x <= minPos.x + body.radius)
        	{
                body.position.x = minPos.x + body.radius;
                body.velocity.x = -body.velocity.x * body.bounciness;
        	}
        	if(body.position.y <= minPos.y + body.radius)
        	{
                body.position.y = minPos.y + body.radius;
                body.velocity.y = -body.velocity.y * body.bounciness;
        	}
            if (body.position.x >= maxPos.x - body.radius)
            {
                body.position.x = maxPos.x - body.radius;
                body.velocity.x = -body.velocity.x * body.bounciness;
            }
            if (body.position.y >= maxPos.y - body.radius)
            {
                body.position.y = maxPos.y - body.radius;
                body.velocity.y = -body.velocity.y * body.bounciness;
            }
            bodyManager_.SetComponent(entity, body);
        }
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity,
                                                   static_cast<core::EntityMask>(core::ComponentType::BODY2D) |
                                                   static_cast<core::EntityMask>(core::ComponentType::CIRCLE_COLLIDER2D)) ||
                entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::DESTROYED)))
                continue;
            for (core::Entity otherEntity = entity; otherEntity < entityManager_.GetEntitiesSize(); otherEntity++)
            {
                if (entity == otherEntity)
                    continue;
                if (!entityManager_.HasComponent(otherEntity,
                                                 static_cast<core::EntityMask>(core::ComponentType::BODY2D) | static_cast<core::EntityMask>(core::ComponentType::CIRCLE_COLLIDER2D)) ||
                    entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::DESTROYED)))
                    continue;
                 CircleBody& body1 = bodyManager_.GetComponent(entity);

                 CircleBody& body2 = bodyManager_.GetComponent(otherEntity);

               if(BodyIntersect(body1, body2))
               {
                   ResolveBodyIntersect(body1, body2);
                   onTriggerAction_.Execute(entity, otherEntity);
               }
                   

            }
        }
    }

    void PhysicsManager::SetBody(core::Entity entity, const CircleBody& body)
    {
        bodyManager_.SetComponent(entity, body);
    }

    const CircleBody& PhysicsManager::GetBody(core::Entity entity) const
    {
        return bodyManager_.GetComponent(entity);
    }

    void PhysicsManager::AddBody(core::Entity entity)
    {
        bodyManager_.AddComponent(entity);
    }

    void PhysicsManager::AddBox(core::Entity entity)
    {
        boxManager_.AddComponent(entity);
    }

    void PhysicsManager::SetBox(core::Entity entity, const Box& box)
    {
        boxManager_.SetComponent(entity, box);
    }

    const Box& PhysicsManager::GetBox(core::Entity entity) const
    {
        return boxManager_.GetComponent(entity);
    }

    void PhysicsManager::AddCircle(core::Entity entity)
    {
        circleManager_.AddComponent(entity);
    }

    void PhysicsManager::SetCircle(core::Entity entity, const CircleBody& circle)
    {
        circleManager_.SetComponent(entity, circle);
    }

    const CircleBody& PhysicsManager::GetCircle(core::Entity entity) const
    {
        return circleManager_.GetComponent(entity);
    }

    void PhysicsManager::RegisterTriggerListener(OnTriggerInterface& collisionInterface)
    {
       
    }

    void PhysicsManager::CopyAllComponents(const PhysicsManager& physicsManager)
    {
        bodyManager_.CopyAllComponents(physicsManager.bodyManager_.GetAllComponents());
        boxManager_.CopyAllComponents(physicsManager.boxManager_.GetAllComponents());
    }
}
