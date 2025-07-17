#pragma once

#include "CommandTraits.hpp"

#include <cstdint>
#include <iosfwd>

namespace sw::io
{
	struct CreateMap
	{
		uint32_t width{};
		uint32_t height{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("width", width);
			visitor.visit("height", height);
		}
	};

	template<>
	struct CommandTraits<CreateMap>
	{
		static constexpr const char* getName()
		{
			return "CREATE_MAP";
		}
	};
}
