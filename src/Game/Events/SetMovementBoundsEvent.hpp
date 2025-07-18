#pragma once

#include "Common.hpp"

#include <cstdint>

namespace sw::game
{
	struct SetMovementBoundsEvent
	{
		uint32_t w{};
		uint32_t h{};
	};
}
