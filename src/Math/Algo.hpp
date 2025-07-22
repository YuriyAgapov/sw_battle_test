#pragma once

#include "Vector2.hpp"

namespace sw::math
{
	/**
	 * @brief Visits all cells within a circle with a specified radius from the centre.
	 *
	 * Used to visit a circular area on an integer grid (tiles).
	 *
	 * @param centre The centre of the circle (in grid coordinates).
	 * @param radius The radius in cells (inclusive).
	 * @param func The function called for each suitable cell.
	 */
	inline bool foreachCircle(const Vector2& center, const int64_t minRadius, const int64_t maxRadius, std::function<bool (const Vector2&)> func)
	{
		const int64_t squaredMinRadius = minRadius * minRadius;
		const int64_t squaredMaxRadius = maxRadius * maxRadius;

		for (int64_t y = -maxRadius; y <= maxRadius; ++y)
		{
			for (int64_t x = -maxRadius; x <= maxRadius; ++x)
			{
				const double squaredDist = x*x + y*y;
				if (squaredDist >= squaredMinRadius && squaredDist <= squaredMaxRadius)
				{
					if (!func(center + Vector2{x, y}))
					{
						return false;
					}
				}
			}
		}
		return true;
	}

	/**
	 * @brief Visits all cells within a rectangle around the centre.
	 *
	 * Used to visit a square area on a grid.
	 * The range is symmetrical: `range` cells on each side of the centre.
	 *
	 * @param centre The centre of the rectangle.
	 * @param range The half-radius in cells.
	 * @param func The function called for each cell.
	 */
	inline bool foreachRect(const Vector2& center, const int64_t range, std::function<bool (const Vector2&)> func)
	{
		for (int64_t y = -range; y <= range; ++y)
		{
			for (int64_t x = -range; x <= range; ++x)
			{
				auto p = center + Vector2{x, y};
				if (!func(p))
				{
					return false;
				}
			}
		}
		return true;
	}
}
