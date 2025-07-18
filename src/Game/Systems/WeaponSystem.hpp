#pragma once

#include <ECS/System.hpp>

namespace sw::game
{
	struct WeaponSystem : public ecs::System
	{
	public:
		explicit WeaponSystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;
	};
}
