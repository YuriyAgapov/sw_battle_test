#pragma once

#include "Common.hpp"
#include "ECS/Component.hpp"
#include "Game/DamageType.hpp"
#include "Game/UnitType.hpp"

#include <unordered_map>
#include <unordered_set>

namespace sw::game
{
	struct Weapon
	{
		uint32_t damage{};
		uint32_t minRange{};
		uint32_t maxRange{};
		DamageType type = DamageType::None;
		std::unordered_set<UnitType> canDamage;
		uint32_t targetId = InvalidId;
	};

	using WeaponMap = std::unordered_map<uint32_t, game::Weapon>;

	struct WeaponComponent : public ecs::Component
	{
		// weapon id - weapon
		std::unordered_map<uint32_t, Weapon> weapons;
	};
}


