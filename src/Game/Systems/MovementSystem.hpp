#pragma once

#include <ECS/System.hpp>

namespace sw::game
{
	struct MovementSystem : public ecs::System
	{
	public:
		explicit MovementSystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;
	};
}
