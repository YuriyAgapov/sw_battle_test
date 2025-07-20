#pragma once

#include <ECS/Context.hpp>
#include <Game/Components/GridComponent.hpp>

namespace sw::debug
{
	template <typename T>
	inline void check(T expr, const std::string& msg)
	{
		//todo: disable with conditional compilation
		if (!expr)
			throw std::runtime_error(msg);
	}

	inline void checkPosition(const std::shared_ptr<ecs::Context>& context, const math::Vector2& pos)
	{
		check(context, "invalid context");

		auto grid = context->getSingletoneComponent<game::GridComponent>();
		check(grid, "invalid grid");
		check(grid->bounds.contains(pos), "point out of bounds");
	}
}
