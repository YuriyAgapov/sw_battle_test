#pragma once

#include <cstdint>

namespace sw::game
{
	// point of improvements: use dictionary from meta
	enum class DamageType : uint8_t
	{
		None,
		Regular,
		Heal,
	};
}
