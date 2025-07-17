#pragma once

#include <ECS/Component.hpp>

namespace sw::game
{
	struct MeleeAttackComponent : public ecs::Component
	{
		uint32_t strength{1};
		uint32_t range{1};
	};
}


