#pragma once

#include <ECS/System.hpp>

namespace sw::game
{
	struct DamageCommand;

	class DamageSystem : public ecs::System
	{
	public:
		explicit DamageSystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;

	private:
		void performDamage(const DamageCommand& command);
		uint32_t calcDamage(const DamageCommand& command) const;
		uint32_t calcHeal(const DamageCommand& command) const;
	};
}
