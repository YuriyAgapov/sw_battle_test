#pragma once

#include "Math/Vector2.hpp"

#include <optional>
#include <vector>

namespace sw::game
{
	struct BehaviourComponent
	{
		uint32_t priority{};
		std::optional<math::Vector2> target;
	};
}
