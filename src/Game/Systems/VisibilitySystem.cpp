#include "VisibilitySystem.hpp"

#include "Debug.hpp"
#include "Game/Components/GridComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/ViewerComponent.hpp"
#include "Game/Components/VisibleComponent.hpp"
#include "Math/Algo.hpp"

#include <ECS/Context.hpp>

namespace sw::game
{
	VisibilitySystem::VisibilitySystem(const std::shared_ptr<ecs::Context>& context) :
			ecs::System(context)
	{
		debug::check(context, "invalid context");
	}

	void VisibilitySystem::advance()
	{
		context->for_each<ViewerComponent, MovementComponent>(
			[this, grid = context->getSingletoneComponent<GridComponent>()](
				const uint32_t entityId, auto viewer, auto movement)
			{
				viewer->visibleMapping.clear();
				math::foreachRect(
					movement->pos,
					viewer->range,
					[this, grid, viewer, selfId = entityId](const math::Vector2& point)
					{
						if (!grid->bounds.contains(point))
							return false;

						for (const uint32_t entityId : grid->mapping.get(point))
						{
							if (selfId == entityId)
								continue;
							auto visible = context->getComponent<VisibleComponent>(entityId);
							//poi: calc visible coef by factors
							if (visible->type == VisibleType::Visible)
								viewer->visibleMapping.add(entityId, point);
						}
						return true;
					});
				return true;
			});
	}
}
