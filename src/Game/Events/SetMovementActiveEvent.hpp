#pragma once

#include "Common.hpp"

#include <cstdint>

namespace sw::game
{
	struct SetMovementActiveEvent
	{
		uint32_t causerId = InvalidId;
		bool active = true;
	};
}
