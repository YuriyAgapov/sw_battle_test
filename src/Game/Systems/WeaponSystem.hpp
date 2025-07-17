#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/WeaponComponent.hpp"
#include "Game/Events/DamageEvent.hpp"

#include <Game/Events/UseWeaponEvent.hpp>

namespace sw::game
{
	struct WeaponSystem : public ecs::System
	{
	public:
		WeaponSystem(const std::shared_ptr<ecs::Context>& context) :
				context(context)
		{
			context->getDispatcher().subscribe<UseWeaponEvent>([this](const UseWeaponEvent& weaponEvent)
															   { UseWeapon(weaponEvent); });
		}

		void advance() final
		{
			context->for_each<WeaponComponent>(
				[this](auto entity, auto weaponComponent)
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
		void UseWeapon(const UseWeaponEvent& weaponEvent)
		{
			auto weaponComponent = context->getComponent<WeaponComponent>(weaponEvent.causerId);
			auto weaponIter = weaponComponent->weapons.find(weaponEvent.weaponId);
			if (weaponIter != weaponComponent->weapons.end())
			{
				weaponIter->second.targetId = weaponEvent.targetId;
			}
		}

		std::shared_ptr<ecs::Context> context;
	};
}
