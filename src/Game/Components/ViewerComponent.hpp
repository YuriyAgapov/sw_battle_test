#pragma once

#include "Game/Types/EntityMapping.hpp"

#include <cstdint>

namespace sw::game
{
	struct ViewerComponent
	{
		// poi: introduce stats compoennet
		uint32_t range{};

		// poi: introduce caching and skip unnecessary updates
		EntityMapping visibleMapping;
		// poi: view power, filer tags etc
	};
}
