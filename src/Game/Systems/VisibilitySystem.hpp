#pragma once

#include <ECS/System.hpp>

namespace sw::game
{
	class VisibilitySystem : public ecs::System
	{
	public:
		explicit VisibilitySystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;
	};
}
