#pragma once

#include <memory>

namespace sw::ecs
{
	class Context;

	class System
	{
	public:
		explicit System(const std::shared_ptr<ecs::Context>& context) :
				context(context)
		{}

		virtual ~System() = default;

		virtual void advance() = 0;

	protected:
		std::shared_ptr<ecs::Context> context;
	};
}
