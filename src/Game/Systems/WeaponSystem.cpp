#include "WeaponSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/WeaponComponent.hpp"
#include "Game/Events/DamageEvent.hpp"

#include <ECS/Context.hpp>

namespace sw::game
{

	WeaponSystem::WeaponSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{
		debug::check(context, "invalid context");
	}

	void WeaponSystem::advance()
	{
		context->for_each<WeaponComponent>(
			[this](const uint32_t entityId, auto weaponComponent)
			{
				for (auto& weapon : weaponComponent->weapons)
				{
					if (weapon.targetId != InvalidId)
					{
						context->getDispatcher() << DamageEvent{
																entityId, weapon.targetId, weapon.damage, weapon.damageType, weapon.weaponType};

						// reset target id
						weapon.targetId = InvalidId;
					}
				}
				return true;
			});
	}

}
