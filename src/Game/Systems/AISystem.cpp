#include "AISystem.hpp"

#include "Debug.hpp"
#include "Game/Components/BehaviourComponent.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/GridComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/ViewerComponent.hpp"
#include "Game/Components/WeaponComponent.hpp"
#include "Game/Events/SetMovementTargetEvent.hpp"
#include "Math/Algo.hpp"

#include <ECS/Context.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/System/EventLog.hpp>

namespace sw::game
{
	static bool isValidTarget(const std::shared_ptr<ecs::Context>& context, const uint32_t entityId, const Weapon& weapon)
	{
		auto [damageTaker, movement] = context->getComponents<DamageTakerComponent, MovementComponent>(entityId);
		if (!damageTaker || !movement)
			return false;

		return damageTaker->health > 0 && weapon.canDamage.contains(movement->type);

	}

	AISystem::AISystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
			, grid(context->getSingletoneComponent<GridComponent>())
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<SetMovementTargetEvent>(
			[this](const SetMovementTargetEvent& event)
			{
				if (!grid->bounds.contains(event.target))
					throw std::runtime_error("Failed to set movement target, point out of bounds");

				auto behaviour = this->context->getComponent<BehaviourComponent>(event.entityId);
				behaviour->target = event.target;

				EventLog::log(
					this->context->getTickCount(),
					io::MarchStarted{event.entityId, event.target.getX(), event.target.getY()});
			});
	}

	void AISystem::advance()
	{
		// simple a desision making
		context->for_each<BehaviourComponent, MovementComponent>(
			[this](const uint32_t entityId, auto behaviour, auto movement)
			{
				for (const auto priority : behaviour->priorityOrder)
				{
					switch (priority)
					{
						case BehaviourPriorityType::Attack:
							if (DoAttack(entityId, movement->pos))
							{
								// to pause movement
								movement->velocity = {};
								return true;
							}
						case BehaviourPriorityType::Movement:
							if (behaviour->target)
							{
								const math::Vector2 target = *behaviour->target;
								if (movement->pos == target)
								{
									// arrived, reset state
									behaviour->target.reset();
									movement->velocity = {};

									// report to log
									EventLog::log(
										context->getTickCount(),
										io::MarchEnded{entityId, movement->pos.getX(), movement->pos.getY()});
								}
								else
								{
									uint32_t speed = 1;	 // poi: make speed attribute
									auto pos2 = math::moveTo(movement->pos, target, speed);
									movement->velocity = pos2 - movement->pos;

									if (!movement->velocity.isZero())
									{
										return true;
									}
								}
							}
							break;
					}
				}
				return true;
			});
	}

	bool AISystem::DoAttack(const uint32_t entityId, const math::Vector2& pos)
	{
		auto weaponComponent = context->getComponent<WeaponComponent>(entityId);

		// find sutable weapon
		for (auto& [weaponId, weapon] : weaponComponent->weapons)
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

		if (weapon.weaponType== WeaponType::Melee)
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
				weapon.maxRange,
				[viewer, &targetId, pos, minRange = weapon.minRange, weapon, this](const math::Vector2& point)
				{
					if (math::length(pos - point) >= minRange)
					{
						for (const uint32_t entityId : viewer->visibleMapping.get(pos))
						{
							if (isValidTarget(context, entityId, weapon))
							{
								// store id and interrupt
								targetId = entityId;
								return false;
							}
						}
					}
					return true;
				});
		}
		return targetId;
	}
}
