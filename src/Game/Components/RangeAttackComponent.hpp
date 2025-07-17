#pragma once

#include "ECS/Component.hpp"

namespace sw::game
{
	struct RangeAttackComponent : public ecs::Component
	{
		uint32_t range{1};
		uint32_t agility{1};
	};
}
