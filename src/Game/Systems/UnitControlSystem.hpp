#pragma once

#include <ECS/System.hpp>
#include <Math/Vector2D.hpp>

namespace sw::game
{
	struct Weapon;

	class UnitControlSystem : public ecs::System
	{
	public:
		explicit UnitControlSystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;

	private:
		bool DoAttack(const uint32_t entityId, const math::Vector2u& pos);

		uint32_t findTarget(const uint32_t selfId, const math::Vector2u& pos, const Weapon& weapon) const;
	};
}
