#pragma once

#include "Common.hpp"

#include <cstdint>

#include <Math/Vector2.hpp>

namespace sw::game
{
	struct MoveCommand
	{
		uint32_t unitId = InvalidId;
		math::Vector2 velocity;
	};
}
