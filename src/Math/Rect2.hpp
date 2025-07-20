#pragma once

#include "Vector2.hpp"

namespace sw::math
{
	/**
	 * @brief Axis-aligned rectangle defined by minimum and maximum corners.
	 *
	 * Represents a rectangular area on a 2D integer grid.
	 * Assumes inclusive bounds: a point on the edge is considered inside.
	 */
	class Rect2
	{
	public:
		Rect2() = default;

		Rect2(const uint32_t width, const uint32_t height) :
				min{0, 0},
				max(width, height)
		{}

		/**
		 * @brief Checks if the given position is inside the rectangle (inclusive).
		 *
		 * @param pos The position to check.
		 * @return true if pos is inside or on the border of the rectangle.
		 * @return false otherwise.
		 */
		bool contains(const math::Vector2& pos) const
		{
			return pos.x >= min.x && pos.x <= max.x && pos.y >= min.y && pos.y <= max.y;
		}

	private:
		math::Vector2 min;
		math::Vector2 max;
	};
}
