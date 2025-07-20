#pragma once

#include "Common.hpp"
#include "Game/Types/DamageType.hpp"
#include "Game/Types/DispositionType.hpp"
#include "Game/Types/WeaponType.hpp"

#include <unordered_map>
#include <unordered_set>

namespace sw::game
{
	struct Weapon
	{
		// todo: move to the game params
		static constexpr uint32_t swordId = 1u;
		static constexpr uint32_t bowId = 2u;

		uint32_t damage{};
		uint32_t minRange{};
		uint32_t maxRange{};
		DamageType damageType = DamageType::Regular;
		WeaponType weaponType = WeaponType::Melee;
		std::unordered_set<DispositionType> canDamage;
		uint32_t targetId = InvalidId;
	};

	using WeaponMap = std::unordered_map<uint32_t, game::Weapon>;

	struct WeaponComponent
	{
		// weapon id - weapon
		std::unordered_map<uint32_t, Weapon> weapons;
	};
}


