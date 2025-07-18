#pragma once

#include "ECS/Component.hpp"

namespace sw::game
{
	struct DamageTakerComponent : public ecs::Component
	{
		uint32_t health{1};
		uint32_t maxHealth{1};

		DamageTakerComponent(const uint32_t maxHealth) :
				health(maxHealth),
				maxHealth(maxHealth)
		{}
	};
}
