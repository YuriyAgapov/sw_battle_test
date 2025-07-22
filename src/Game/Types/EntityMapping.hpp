#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include <Math/Vector2.hpp>

namespace sw::game
{
	class EntityMapping
	{
	public:
		void add(const uint32_t id, const math::Vector2& pos)
		{
			tiles[pos].emplace(id);
		}

		void remove(const uint32_t id, const math::Vector2& pos)
		{
			if (auto iter = tiles.find(pos); iter != tiles.end())
			{
				iter->second.erase(id);
				if (iter->second.empty())
				{
					tiles.erase(iter);
				}
			}
		}

		void move(const uint32_t id, const math::Vector2& oldPos, const math::Vector2& newPos)
		{
			// todo: check for exists
			remove(id, oldPos);
			add(id, newPos);
		}

		std::unordered_set<uint32_t> get(const math::Vector2& pos) const
		{
			// poi: avoid copy
			auto iter = tiles.find(pos);
			return iter != tiles.end() ? iter->second : std::unordered_set<uint32_t>{};
		}

		void clear()
		{
			tiles.clear();
		}
	private:
		std::unordered_map<math::Vector2, std::unordered_set<uint32_t>> tiles;
		//TODO: use R-Tree
	};
}
