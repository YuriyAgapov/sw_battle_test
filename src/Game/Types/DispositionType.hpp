#pragma once

#include <cstdint>

namespace sw::game
{
	// point of improvements: use dictionary from meta
	enum class DispositionType : uint8_t
	{
		Ground,
		Air,
		Underground,
	};
}
