#pragma once

#include "ECS/Component.hpp"
#include "Game/BehaviourPriorityType.hpp"
#include "Game/UnitType.hpp"
#include "Math/Vector2D.hpp"

#include <vector>

namespace sw::game
{
	struct UnitComponent : public ecs::Component
	{
		UnitType type = UnitType::Stationary;
		math::Vector2u pos{};
		std::vector<BehaviourPriorityType> priorityOrder = {BehaviourPriorityType::Attack, BehaviourPriorityType::Movement};
	};
}
