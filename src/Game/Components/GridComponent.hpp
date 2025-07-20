#pragma once

#include "Game/Types/EntityMapping.hpp"

#include <Math/Rect2.hpp>

namespace sw::game
{
	/**
	 * @struct GridComponent
	 * @brief ECS component that holds spatial grid data for an entity.
	 *
	 * This component wraps a Grid instance which manages positions of entities
	 * on a 2D tile-based grid. It can be attached to an entity to enable spatial
	 * queries and grid-based logic.
	 */
	struct GridComponent
	{
		EntityMapping mapping;
		math::Rect2 bounds;
	};
}
