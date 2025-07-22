#include "MovementSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/GridComponent.hpp"
#include "Game/Components/MovementComponent.hpp"

#include <ECS/Context.hpp>
#include <Game/Commands/MoveCommand.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>

namespace sw::game
{
	MovementSystem::MovementSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<io::CreateMap>(
			[context](const io::CreateMap& command)
			{
				auto grid = context->addSingletoneComponent<game::GridComponent>();
				grid->bounds = math::Rect2(command.width, command.height);

				debug::check(grid->bounds.isValid(), "Failed to CreateMap, bounds is invalid");

				context->getDispatcher() << io::MapCreated{command.width, command.height};
			});
		context->getDispatcher().subscribe<io::UnitSpawned>(
			[context](const io::UnitSpawned& event)
			{
				auto movement = context->getComponent<MovementComponent>(event.unitId);
				auto grid = context->getSingletoneComponent<game::GridComponent>();

				debug::check(movement, "invalid movement");
				debug::check(grid, "invalid grid");

				grid->mapping.add(event.unitId, movement->pos);
			});
		context->getDispatcher().subscribe<io::UnitDied>(
			[context](const io::UnitDied& event)
			{
				auto movement = context->getComponent<MovementComponent>(event.unitId);
				auto grid = context->getSingletoneComponent<game::GridComponent>();

				debug::check(movement, "invalid movement");
				debug::check(grid, "invalid grid");

				grid->mapping.remove(event.unitId, movement->pos);
			});

		context->getDispatcher().subscribe<MoveCommand>(
			[context](const MoveCommand& command)
			{
				// skip died
				if (context->hasPendingKill(command.unitId))
					return;

				auto movement = context->getComponent<MovementComponent>(command.unitId);
				auto grid = context->getSingletoneComponent<game::GridComponent>();

				debug::check(movement, "invalid movement");
				debug::check(grid, "invalid grid");

				grid->mapping.remove(command.unitId, movement->pos);
				movement->pos = movement->pos + command.velocity;
				grid->mapping.add(command.unitId, movement->pos);

				context->getDispatcher() << io::UnitMoved{command.unitId, movement->pos.getX(), movement->pos.getY()};
			});

	}

	void MovementSystem::advance()
	{
		// nothing
	}
}
