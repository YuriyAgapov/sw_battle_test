#pragma once

#include "Common.hpp"

#include <cstdint>

namespace sw::game
{
	struct DeathEvent
	{
		uint32_t entityId = InvalidId;
	};
}
