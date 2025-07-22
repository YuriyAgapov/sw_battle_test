#pragma once

#include "Game/Types/DispositionType.hpp"
#include "Math/Vector2.hpp"

namespace sw::game
{
	struct MovementComponent
	{
		DispositionType type = DispositionType::Ground;
		math::Vector2 pos{};
		uint32_t speed{};
	};
}
