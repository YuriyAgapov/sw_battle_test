#pragma once

#include "Common.hpp"

#include <cstdint>
#include <functional>

namespace sw::ecs
{
	/** Component index, a combination of entity identifier and component type hash
	 *  Used by context to address components
	 */
	struct ComponentIndex
	{
		uint32_t enitityId = InvalidId;
		size_t componentTypeId = 0;

		bool operator==(const ComponentIndex& other) const noexcept
		{
			return enitityId == other.enitityId &&
				   componentTypeId == other.componentTypeId;
		}
		std::size_t getHash() const
		{
			std::size_t h1 = std::hash<uint32_t>{}(enitityId);
			std::size_t h2 = std::hash<size_t>{}(componentTypeId);
			return h1 ^ (h2 << 1);
		}
	};
}

template<>
struct std::hash<sw::ecs::ComponentIndex>
{
	std::size_t operator()(const sw::ecs::ComponentIndex& index) const noexcept
	{
		return index.getHash();
	}
};
