#pragma once

#include "CommandTraits.hpp"

#include <cstdint>
#include <iosfwd>

namespace sw::io
{
	struct SpawnHunter
	{
		uint32_t unitId{};
		uint32_t x{};
		uint32_t y{};
		uint32_t hp{};
		uint32_t agility{};
		uint32_t strength{};
		uint32_t range{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("x", x);
			visitor.visit("y", y);
			visitor.visit("hp", hp);
			visitor.visit("agility", agility);
			visitor.visit("strength", strength);
			visitor.visit("range", range);
		}
	};

	template<>
	struct CommandTraits<SpawnHunter>
	{
		static constexpr const char* getName()
		{
			return "SPAWN_HUNTER";
		}
	};
}
