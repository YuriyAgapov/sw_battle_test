#include "AISystem.hpp"

#include "Debug.hpp"
#include "Game/Components/BehaviourComponent.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/ViewerComponent.hpp"
#include "Game/Components/WeaponComponent.hpp"
#include "Math/Algo.hpp"

#include <ECS/Context.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>

#include <algorithm>

namespace sw::game
{
	static bool isValidTarget(
		const std::shared_ptr<ecs::Context>& context, const uint32_t entityId, const Weapon& weapon)
	{
		auto [damageTaker, movement] = context->getComponents<DamageTakerComponent, MovementComponent>(entityId);
		if (!damageTaker || !movement)
		{
			return false;
		}

		return damageTaker->health > 0 && weapon.canDamage.contains(movement->type);
	}

	AISystem::AISystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<io::March>(
			[context](const io::March& event)
			{
				const math::Vector2 target{event.targetX, event.targetY};

				debug::checkPosition(context, target);

				auto [behaviour, movement]
					= context->getComponents<BehaviourComponent, MovementComponent>(event.unitId);
				behaviour->target = target;

				context->getDispatcher() << io::MarchStarted{
															 event.unitId, movement->pos.getX(), movement->pos.getY(), event.targetX, event.targetY};
			});
		context->getDispatcher().subscribe<io::UnitMoved>(
			[context](const io::UnitMoved& event)
			{
				auto [behaviour, movement]
					= context->getComponents<BehaviourComponent, MovementComponent>(event.unitId);
				if (!behaviour->target)
				{
					return;
				}

				if (movement->pos == *behaviour->target)
				{
					// arrived, reset state
					behaviour->target.reset();
					movement->velocity = {};

					// report to log
					context->getDispatcher()
						<< io::MarchEnded{event.unitId, movement->pos.getX(), movement->pos.getY()};
				}
			});
	}

	void AISystem::advance()
	{
		auto view = context->makeView<BehaviourComponent, MovementComponent>();
		std::sort(
			view.begin(),
			view.end(),
			[](const auto& left, const auto& right)
			{
				return std::get<1>(left)->priority < std::get<1>(right)->priority;
			});

		for (const auto& [entityId, behaviour, movement] : view)
		{
			if (DoAttack(entityId, movement->pos))
			{
				// to pause movement
				movement->velocity = {};
			}
			else if (behaviour->target)
			{
				uint32_t speed = 1;	 // poi: make speed attribute
				movement->velocity = math::makeVelocity(movement->pos, *behaviour->target, speed);

				if (!movement->velocity.isZero())
				{
					continue;
				}
			}
		}
	}

	bool AISystem::DoAttack(const uint32_t entityId, const math::Vector2& pos)
	{
		auto weaponComponent = context->getComponent<WeaponComponent>(entityId);

		// find sutable weapon
		for (auto& weapon : weaponComponent->weapons)
		{
			if (const uint32_t targetId = findTarget(entityId, pos, weapon); targetId != InvalidId)
			{
				weapon.targetId = targetId;
				return true;
			}
		}
		return false;
	}

	uint32_t AISystem::findTarget(const uint32_t selfId, const math::Vector2& pos, const Weapon& weapon) const
	{
		uint32_t targetId = InvalidId;

		auto viewer = context->getComponent<ViewerComponent>(selfId);

		if (weapon.weaponType == WeaponType::Melee)
		{
			// search nearby in area
			math::foreachRect(
				pos,
				weapon.maxRange,
				[viewer, &targetId, this, weapon](const math::Vector2& point)
				{
					for (const uint32_t entityId : viewer->visibleMapping.get(point))
					{
						if (isValidTarget(context, entityId, weapon))
						{
							// store id and interrupt
							targetId = entityId;
							return false;
						}
					}
					return true;
				});
		}
		else if (weapon.weaponType == WeaponType::Range)
		{
			// search nearby in range
			math::foreachCircle(
				pos,
				weapon.minRange,
				weapon.maxRange,
				[viewer, &targetId, weapon, this](const math::Vector2& point)
				{
					for (const uint32_t entityId : viewer->visibleMapping.get(point))
					{
						if (isValidTarget(context, entityId, weapon))
						{
							// store id and interrupt
							targetId = entityId;
							return false;
						}
					}
					return true;
				});
		}
		return targetId;
	}
}
