#pragma once

#include "Common.hpp"
#include "Math/Vector2.hpp"

#include <optional>

namespace sw::game
{
	struct BehaviourComponent
	{
		uint32_t priority{};
		uint32_t targetId = InvalidId;
		std::optional<math::Vector2> waypoint;
	};
}
