#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/RangeAttackComponent.hpp"

namespace sw::game
{
	struct RangeAttackSystem : public ecs::System
	{
	public:
		RangeAttackSystem(std::shared_ptr<ecs::Context> context) :
				context(context)
		{}

		void advance()
		{
			context->for_each<RangeAttackComponent>(
				[](auto rangeAttackComponent)
				{
					//...
					return true;
				});
		}

	private:
		std::shared_ptr<ecs::Context> context;
	};
}
