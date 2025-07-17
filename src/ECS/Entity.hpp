#pragma once

#include <cstdint>
#include <unordered_map>

namespace sw::ecs
{
	struct Entity
	{
		uint32_t id{};

		// type - id
		std::unordered_map<size_t, uint32_t> components;

		bool deleteLater = false;
	};
}
