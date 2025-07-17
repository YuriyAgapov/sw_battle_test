#pragma once

#include <cstdint>

namespace sw::game
{
	// point of improvements: use dictionary from meta
	enum class BehaviourPriorityType : uint8_t
	{
		Attack,
		Movement
	};
}
