#pragma once

#include "Game/Types/EntityMapping.hpp"

#include <Math/Rect2.hpp>

namespace sw::game
{
	struct GridComponent
	{
		EntityMapping mapping;
		math::Rect2 bounds;
	};
}
