#pragma once

#include "ECS/Component.hpp"
#include "Math/Vector2D.hpp"

#include <optional>

namespace sw::game
{
	struct MovementComponent : public ecs::Component
	{
		bool active{true};
		std::optional<math::Vector2u> target;
		uint32_t speed{1};

		bool canMove() const
		{
			return active && speed > 0 && target;
		}
	};
}
