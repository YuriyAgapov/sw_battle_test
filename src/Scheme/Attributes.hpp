#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace sw::scheme
{
	enum class AttributeType : uint8_t
	{
		UnitId,
		X,
		Y,
		HP,
		Agility,
		Strength,
		Range,
		AttackType,
		Blocking,
	};

	using AttributeValues = std::unordered_map<AttributeType, uint32_t>;
	using AttributeSet = std::vector<AttributeType>;

	const char* getAttributeName(const AttributeType Id);
}
