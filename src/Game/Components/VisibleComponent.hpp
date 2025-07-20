#pragma once

#include "Game/Types/VisibleType.hpp"

namespace sw::game
{
	struct VisibleComponent
	{
		VisibleType type = VisibleType::Visible;
		// poi: visible factor, tags
	};
}
