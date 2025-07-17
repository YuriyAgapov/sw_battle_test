#pragma once

#include "ECS/Component.hpp"
#include "Math/Vector2D.hpp"

#include <optional>

namespace sw::game
{
	struct UnitMovementComponent : public ecs::Component
	{
		std::optional<math::Vector2u> target;
		uint32_t speed{};
	};
}
