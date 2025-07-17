#pragma once

#include "ComponentIndex.hpp"

namespace sw::ecs
{
	struct Component
	{
		ComponentIndex id;

		virtual ~Component() = default;
	};
}
