#pragma once

#include <cstdint>
#include <iosfwd>

namespace sw::io
{
	struct SpawnHealer
	{
		constexpr static const char* Name = "SPAWN_HEALER";

		uint32_t unitId{};
		uint32_t x{};
		uint32_t y{};
		uint32_t spirit{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("x", x);
			visitor.visit("y", y);
			visitor.visit("spirit", spirit);
		}
	};
}
