#pragma once

#include <ECS/System.hpp>

namespace sw::game
{
	class SpawnUnitSystem : public ecs::System
	{
	public:
		explicit SpawnUnitSystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;

	private:
		// used for ordering of the units
		uint32_t globalOrder = 0;
	};
}
