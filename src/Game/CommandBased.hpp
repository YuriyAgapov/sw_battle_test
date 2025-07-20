#pragma once

#include "Math/Algo.hpp"
#include <cstdint>

#include <Math/Rect2.hpp>
#include <Math/Vector2.hpp>

#include <Game/Types/EntityMapping.hpp>

namespace sw::game
{
	/*struct Weapon
	{
		uint32_t damage{};
		uint32_t minRange{};
		uint32_t maxRange{};
	};

	struct Unit

	struct Unit
	{
		uint32_t id{};
		uint32_t health{};
		math::Vector2 pos;
		Weapon melee;
		Weapon range;
	};

	struct World
	{
		math::Rect2 bounds;
		EntityMapping mapping;
	};

	struct HunterLogic
	{
		void advance()
		{
			Unit unit;
			World world;
			math::foreachRect(unit.pos, 1, [world](const math::Vector2& point)
							  {
				for (const uint32_t entityId : world.mapping.get(point))
{
					if ()
				}
				return true;
							  });
		}
	};*/
}
