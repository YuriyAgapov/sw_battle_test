#pragma once

#include <cstdint>

namespace sw::game
{
	// point of improvements: use dictionary from meta
	enum class UnitType : uint8_t
	{
		Stationary,
		Underground,
		Ground,
		Flying,
	};
}
