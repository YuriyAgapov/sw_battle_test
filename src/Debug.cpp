#include "Debug.hpp"

#include <ECS/Context.hpp>
#include <Game/Components/GridComponent.hpp>

namespace sw::debug
{
	void checkPosition(const std::shared_ptr<ecs::Context>& context, const math::Vector2& pos)
	{
		check(context, "invalid context");

		auto grid = context->getSingletoneComponent<game::GridComponent>();
		check(grid, "invalid grid");
		check(grid->bounds.contains(pos), "point out of bounds");
	}
}
