#pragma once

#include "Common.hpp"
#include "Game/DamageType.hpp"

#include <cstdint>

namespace sw::game
{
	struct DamageEvent
	{
		uint32_t causerId = InvalidId;
		uint32_t targetId = InvalidId;
		uint32_t amount = 0;
		DamageType type = DamageType::None;
	};
}
