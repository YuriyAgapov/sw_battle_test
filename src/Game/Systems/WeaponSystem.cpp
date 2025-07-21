#include "WeaponSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/BehaviourComponent.hpp"
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
		auto view = context->makeView<BehaviourComponent, WeaponComponent>();
		std::sort(
			view.begin(),
			view.end(),
			[](const auto& left, const auto& right)
			{
				return std::get<1>(left)->priority < std::get<1>(right)->priority;
			});

		for (const auto& [entityId, behaviour, weaponComponent] : view)
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
		}
	}

}
