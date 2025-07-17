#pragma once

#include "Common.hpp"

#include <cstdint>

namespace sw::game
{
	struct UseWeaponEvent
	{
		uint32_t causerId = InvalidId;
		uint32_t targetId = InvalidId;
		uint32_t weaponId = InvalidId;
		// singleshot
	};
}
