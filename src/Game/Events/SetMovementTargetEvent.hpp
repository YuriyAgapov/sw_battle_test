#pragma once

#include "Common.hpp"
#include "Math/Vector2D.hpp"

#include <cstdint>

namespace sw::game
{
	struct SetMovementTargetEvent
	{
		uint32_t entityId = InvalidId;
		math::Vector2u target;
	};
}
