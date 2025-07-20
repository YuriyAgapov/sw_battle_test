#pragma once

#include <cstdint>

namespace sw::game
{
	struct DamageTakerComponent
	{
		uint32_t health{1};
		uint32_t maxHealth{1};
	};
}
