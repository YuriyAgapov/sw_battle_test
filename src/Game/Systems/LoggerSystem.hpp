#pragma once

#include <ECS/System.hpp>

namespace sw::game
{
	class LoggerSystem : public ecs::System
	{
	public:
		explicit LoggerSystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;
	};
}
