#pragma once

#include <ECS/System.hpp>
#include <Math/Vector2.hpp>

namespace sw::game
{

	class GridComponent;
	struct Weapon;

	class AISystem : public ecs::System
	{
	public:
		explicit AISystem(const std::shared_ptr<ecs::Context>& context);

		void advance() final;

	private:
		uint32_t TryToUseWeapon(const uint32_t entityId, const math::Vector2& pos) const;
		uint32_t findTarget(const uint32_t entityId, const math::Vector2& pos, const Weapon& weapon) const;
	};
}
