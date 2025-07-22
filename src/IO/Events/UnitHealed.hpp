#pragma once

#include <cstdint>
#include <string>

namespace sw::io
{
	struct UnitHealed
	{
		constexpr static const char* Name = "UNIT_HEALED";

		uint32_t attackerUnitId{};
		uint32_t targetUnitId{};
		uint32_t heal{};
		uint32_t targetHp{};

		template <typename Visitor>
		void visit(Visitor& visitor) const
		{
			visitor.visit("attackerUnitId", attackerUnitId);
			visitor.visit("targetUnitId", targetUnitId);
			visitor.visit("heal", heal);
			visitor.visit("targetHp", targetHp);
		}
	};
}
