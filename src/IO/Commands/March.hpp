#pragma once

#include "CommandTraits.hpp"

#include <cstdint>
#include <iosfwd>

namespace sw::io
{
	struct March
	{
		uint32_t unitId{};
		uint32_t targetX{};
		uint32_t targetY{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("targetX", targetX);
			visitor.visit("targetY", targetY);
		}
	};

	template<>
	struct CommandTraits<March>
	{
		static constexpr const char* getName()
		{
			return "MARCH";
		}
	};
}
