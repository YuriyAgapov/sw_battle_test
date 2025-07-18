#include <ECS/Context.hpp>
#include <Game/Components/WeaponComponent.hpp>
#include <Game/Events/SetMovementBoundsEvent.hpp>
#include <Game/Events/SpawnUnitEvent.hpp>
#include <Game/Systems/DamageSystem.hpp>
#include <Game/Systems/MovementSystem.hpp>
#include <Game/Systems/SpawnUnitSystem.hpp>
#include <Game/Systems/UnitControlSystem.hpp>
#include <Game/Systems/WeaponSystem.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <IO/System/PrintDebug.hpp>
#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
	using namespace sw;

	if (argc != 2)
	{
		throw std::runtime_error("Error: No file specified in command line argument");
	}

	std::ifstream file(argv[1]);
	if (!file)
	{
		throw std::runtime_error("Error: File not found - " + std::string(argv[1]));
	}

	auto context = std::make_shared<ecs::Context>();
	context->addSystem(std::make_unique<game::SpawnUnitSystem>(context));
	context->addSystem(std::make_unique<game::UnitControlSystem>(context));
	context->addSystem(std::make_unique<game::WeaponSystem>(context));
	context->addSystem(std::make_unique<game::DamageSystem>(context));
	context->addSystem(std::make_unique<game::MovementSystem>(context));

	std::cout << "Commands:\n";
	io::CommandParser parser;
	parser
		.add<io::CreateMap>(
			[context](auto command)
			{
				context->getDispatcher() << game::SetMovementBoundsEvent{command.width, command.height};
				printDebug(std::cout, command);
			})
		.add<io::SpawnSwordsman>(
			[context](auto command)
			{
				context->getDispatcher() << game::SpawnSwordsmanUnitEvent(std::move(command));
				printDebug(std::cout, command);
			})
		.add<io::SpawnHunter>(
			[context](auto command)
			{
				context->getDispatcher() << game::SpawnHunterUnitEvent(std::move(command));
				printDebug(std::cout, command);
			})
		.add<io::March>(
			[context](auto command)
			{
				context->getDispatcher() << game::SetMovementTargetEvent{
																		 command.unitId, math::Vector2u{command.targetX, command.targetY}};
				printDebug(std::cout, command);
			});

	parser.parse(file);

	// force dispatch spawn events
	context->getDispatcher().dispatchAll();

	constexpr uint32_t tickLimit = 1000;
	while (!context->getEntities().empty())
	{
		context->advance();

		if (context->getTickCount() >= tickLimit)
		{
			std::cout << "Simulation interupted: Reached tick limit\n";
			break;
		}
	}

	std::cout << "Simulation ended on " << context->getTickCount() << " turn\n";

	// std::cout << "\n\nEvents:\n";

	// eventLog.log(1, io::MapCreated{10, 10});
	// eventLog.log(1, io::UnitSpawned{1, "Swordsman", 0, 0});
	// eventLog.log(1, io::UnitSpawned{2, "Hunter", 9, 0});
	// eventLog.log(1, io::MarchStarted{1, 0, 0, 9, 0});
	// eventLog.log(1, io::MarchStarted{2, 9, 0, 0, 0});
	// eventLog.log(1, io::UnitSpawned{3, "Swordsman", 0, 9});
	// eventLog.log(1, io::MarchStarted{3, 0, 9, 0, 0});

	// eventLog.log(2, io::UnitMoved{1, 1, 0});
	// eventLog.log(2, io::UnitMoved{2, 8, 0});
	// eventLog.log(2, io::UnitMoved{3, 0, 8});

	// eventLog.log(3, io::UnitMoved{1, 2, 0});
	// eventLog.log(3, io::UnitMoved{2, 7, 0});
	// eventLog.log(3, io::UnitMoved{3, 0, 7});

	// eventLog.log(4, io::UnitMoved{1, 3, 0});
	// eventLog.log(4, io::UnitAttacked{2, 1, 5, 0});
	// eventLog.log(4, io::UnitDied{1});
	// eventLog.log(4, io::UnitMoved{3, 0, 6});

	// eventLog.log(5, io::UnitMoved{2, 6, 0});
	// eventLog.log(5, io::UnitMoved{3, 0, 5});

	// eventLog.log(6, io::UnitMoved{2, 5, 0});
	// eventLog.log(6, io::UnitMoved{3, 0, 4});

	// eventLog.log(7, io::UnitAttacked{2, 3, 5, 5});
	// eventLog.log(7, io::UnitMoved{3, 0, 3});

	// eventLog.log(8, io::UnitAttacked{2, 3, 5, 0});
	// eventLog.log(8, io::UnitDied{3});

	return 0;
}
