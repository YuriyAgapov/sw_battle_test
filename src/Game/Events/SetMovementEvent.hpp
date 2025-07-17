#pragma once

#include "Common.hpp"

#include <cstdint>

namespace sw::game
{
	struct SetMovementEvent
	{
		uint32_t causerId = InvalidId;
		bool active = true;
	};
}
