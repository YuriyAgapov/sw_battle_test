#pragma once

#include "Scheme/Abilities.hpp"
#include "Scheme/Attributes.hpp"

namespace sw::scheme
{
	enum class UnitType : uint8_t
	{
		Swordsman,
		Hunter
	};

	struct UnitScheme
	{
		const char* name;
		AttributeValues defaultAttributes;
		AttributeSet configAttributes;
		AbilitySet defaultAbilities;
	};

	const std::unordered_map<UnitType, UnitScheme>& getUnits();

	bool isValidAttributes(const UnitType type, const AttributeValues& values);
}
