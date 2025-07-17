#pragma once

#include "ECS/Component.hpp"

namespace sw::game
{
	struct DamageTakerComponent : public ecs::Component
	{
		uint32_t health{1};
	};
}
