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
		uint32_t targetId = InvalidId;
		std::unordered_set<UnitType> canDamage;
	};

	struct WeaponComponent : public ecs::Component
	{
		// weapon id - weapon
		std::unordered_map<uint32_t, Weapon> weapons;
	};
}


