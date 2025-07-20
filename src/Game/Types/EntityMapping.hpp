#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include <Math/Vector2.hpp>

namespace sw::game
{
	/**
	 * @class EntityMapping
	 * @brief A simple 2D spatial hash grid mapping positions to sets of entity IDs.
	 *
	 * This class allows tracking entity positions in a discrete 2D space using an
	 * unordered map, where each grid cell (math::Vector2) stores a set of entity IDs.
	 * Useful for spatial queries, collision detection, or visibility checks in grid-based systems.
	 */
	class EntityMapping
	{
	public:
		/**
		 * @brief Adds an entity ID to the specified grid position.
		 * @param id The entity ID to add.
		 * @param pos The position to which the entity should be added.
		 */
		void add(const uint32_t id, const math::Vector2& pos)
		{
			tiles[pos].emplace(id);
		}

		/**
		 * @brief Removes an entity ID from the specified grid position.
		 *        If the position is not found or the ID is not present, does nothing.
		 * @param id The entity ID to remove.
		 * @param pos The position from which the entity should be removed.
		 */
		void remove(const uint32_t id, const math::Vector2& pos)
		{
			if (auto iter = tiles.find(pos); iter != tiles.end())
			{
				iter->second.erase(id);
			}
		}

		/**
		 * @brief Moves an entity ID from one position to another.
		 *        Calls remove() and then add(). Does not validate whether the ID was present.
		 * @param id The entity ID to move.
		 * @param oldPos The current position of the entity.
		 * @param newPos The target position for the entity.
		 */
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
		/**
		 * @brief Internal storage mapping grid positions to sets of entity IDs.
		 */
		std::unordered_map<math::Vector2, std::unordered_set<uint32_t>> tiles;
		//TODO: use R-Tree
	};
}
