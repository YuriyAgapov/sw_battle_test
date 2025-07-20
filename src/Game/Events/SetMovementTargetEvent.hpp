#pragma once

#include "Common.hpp"
#include "Math/Vector2.hpp"

#include <cstdint>

namespace sw::game
{
	struct SetMovementTargetEvent
	{
		uint32_t entityId = InvalidId;
		math::Vector2 target;
	};
}
