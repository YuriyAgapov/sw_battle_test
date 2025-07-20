#pragma once

#include <Math/Rect2.hpp>

namespace sw::game
{
	struct SetMovementBoundsEvent
	{
		math::Rect2 bounds;
	};
}
