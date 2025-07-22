#pragma once

#include <cstdint>
#include <iosfwd>

namespace sw::io
{
	struct SpawnTower
	{
		constexpr static const char* Name = "SPAWN_TOWER";

		uint32_t unitId{};
		uint32_t x{};
		uint32_t y{};
		uint32_t power{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("x", x);
			visitor.visit("y", y);
			visitor.visit("power", power);
		}
	};
}
