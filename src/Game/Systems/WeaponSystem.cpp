#include "WeaponSystem.hpp"

#include "Game/Components/WeaponComponent.hpp"
#include "Game/Events/DamageEvent.hpp"

#include <ECS/Context.hpp>
#include <ECS/Entity.hpp>

namespace sw::game
{

	WeaponSystem::WeaponSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{}

	void WeaponSystem::advance()
	{
		context->for_each<WeaponComponent>(
			[this](ecs::Entity& entity, auto weaponComponent)
			{
				for (auto& [weaponId, weapon] : weaponComponent->weapons)
				{
					if (weapon.targetId != InvalidId)
					{
						context->getDispatcher() << DamageEvent{entity.id, weapon.targetId, weapon.damage, weapon.type};

						// reset target id
						weapon.targetId = InvalidId;
					}
				}
				return true;
			});
	}

}
