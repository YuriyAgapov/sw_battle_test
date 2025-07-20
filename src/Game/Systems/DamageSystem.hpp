#pragma once

#include <ECS/System.hpp>

namespace sw::game
{

	class DamageEvent;

	class DamageSystem : public ecs::System
	{
	public:
		explicit DamageSystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;

	private:
		uint32_t calcDamage(const DamageEvent& damageEvent) const;
		uint32_t calcHeal(const DamageEvent& damageEvent) const;
	};
}
