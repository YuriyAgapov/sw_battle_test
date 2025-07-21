#pragma once

#include "Common.hpp"
#include "Game/Types/DamageType.hpp"
#include "Game/Types/DispositionType.hpp"
#include "Game/Types/WeaponType.hpp"

#include <unordered_set>
#include <vector>

namespace sw::game
{
	struct Weapon
	{
		uint32_t damage{};
		uint32_t minRange{};
		uint32_t maxRange{};
		DamageType damageType = DamageType::Regular;
		WeaponType weaponType = WeaponType::Melee;
		std::unordered_set<DispositionType> canDamage;
	};

	struct WeaponComponent
	{
		std::vector<Weapon> weapons;
	};
}


