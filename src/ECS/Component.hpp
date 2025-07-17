#pragma once

#include <cstdint>

namespace sw::ecs
{
	struct Component
	{
		uint64_t id{};

		virtual ~Component() = default;
	};
}
