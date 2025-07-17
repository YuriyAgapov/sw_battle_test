#pragma once

#include "Common.hpp"
#include "Math/Vector2D.hpp"

#include <cstdint>

namespace sw::game
{
	struct MovementTargetEvent
	{
		uint32_t causerId = InvalidId;
		math::Vector2u target;
	};
}
