#pragma once

#include <ECS/System.hpp>

namespace sw::game
{
	class MovementSystem : public ecs::System
	{
	public:
		explicit MovementSystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;
	};
}
