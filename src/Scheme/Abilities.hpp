#pragma once

#include <cstdint>
#include <vector>

namespace sw::scheme
{
	enum class AbilityType : uint8_t
	{
		Move,
		Melee,
		Shoot
	};

	using AbilitySet = std::vector<AbilityType>;
}
