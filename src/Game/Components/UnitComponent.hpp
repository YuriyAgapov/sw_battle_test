#pragma once

#include "Math/Vector2D.hpp"

#include <ECS/Component.hpp>

namespace sw::game
{
	struct UnitComponent : public ecs::Component
	{
		math::Vector2u pos;
		uint8_t actionPoints{};
		bool block = false;
	};
}
