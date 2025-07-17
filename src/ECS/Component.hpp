#pragma once

#include <cstdint>

namespace sw::ecs
{
	struct Component
	{
		uint32_t componentId{};
		uint32_t entityId{};

		virtual ~Component() = default;
	};
}
