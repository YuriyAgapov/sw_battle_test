#pragma once

#include "Common.hpp"
#include "Game/Types/DamageType.hpp"
#include "Game/Types/WeaponType.hpp"

#include <cstdint>

namespace sw::game
{
	struct DamageCommand
	{
		uint32_t causerId = InvalidId;
		uint32_t targetId = InvalidId;
		uint32_t amount = 0;
		DamageType damageType = DamageType::None;
		WeaponType weaponType = WeaponType::None;
	};
}
