#pragma once

#include "Common.hpp"

#include <cstdint>

namespace sw::ecs
{
	struct Entity
	{
		uint32_t id = InvalidId;

		bool deleteLater = false;
	};
}
