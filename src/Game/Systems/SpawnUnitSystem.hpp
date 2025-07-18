#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"

namespace sw::game
{
	struct SpawnUnitSystem : public ecs::System
	{
	public:
		SpawnUnitSystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;

	private:
		std::shared_ptr<ecs::Context> context;
	};
}
