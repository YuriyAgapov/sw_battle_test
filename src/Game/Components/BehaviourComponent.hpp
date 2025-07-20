#pragma once

#include "Game/Types/BehaviourPriorityType.hpp"
#include "Math/Vector2.hpp"

#include <optional>
#include <vector>

namespace sw::game
{
	struct BehaviourComponent
	{
		std::optional<math::Vector2> target;
		std::vector<BehaviourPriorityType> priorityOrder = {BehaviourPriorityType::Attack, BehaviourPriorityType::Movement};
	};
}
