#pragma once

#include "ECS/Component.hpp"

namespace sw::game
{
	struct RangeAttackComponent : public ecs::Component
	{
		uint32_t range{};
		uint32_t agility{};
	};
}
