#pragma once

#include <ECS/System.hpp>

namespace sw::game
{
	struct SpawnUnitSystem : public ecs::System
	{
	public:
		explicit SpawnUnitSystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;
	};
}
