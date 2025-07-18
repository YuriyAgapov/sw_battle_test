#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/WeaponComponent.hpp"
#include "Game/Events/DamageEvent.hpp"

namespace sw::game
{
	struct WeaponSystem : public ecs::System
	{
	public:
		WeaponSystem(const std::shared_ptr<ecs::Context>& context) :
				context(context)
		{
		}

		void advance() final
		{
			context->for_each<WeaponComponent>(
				[this](ecs::Entity& entity, auto weaponComponent)
				{
					for (auto& [weaponId, weapon] : weaponComponent->weapons)
					{
						if (weapon.targetId != InvalidId)
						{
							context->getDispatcher()
								<< DamageEvent{entity.id, weapon.targetId, weapon.damage, weapon.type};

							// reset target id
							weapon.targetId = InvalidId;
						}
					}
					return true;
				});
		}

	private:
		std::shared_ptr<ecs::Context> context;
	};
}
