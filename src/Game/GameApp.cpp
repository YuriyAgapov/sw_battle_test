#include "GameApp.hpp"

#include "Debug.hpp"

#include <Game/Components/BehaviourComponent.hpp>
#include <Game/Components/GridComponent.hpp>
#include <Game/Components/WeaponComponent.hpp>
#include <Game/GameApp.hpp>
#include <Game/Systems/AISystem.hpp>
#include <Game/Systems/DamageSystem.hpp>
#include <Game/Systems/LoggerSystem.hpp>
#include <Game/Systems/MovementSystem.hpp>
#include <Game/Systems/SpawnUnitSystem.hpp>
#include <Game/Systems/VisibilitySystem.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHealer.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Commands/SpawnTower.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/PrintDebug.hpp>
#include <Math/Rect2.hpp>
#include <fstream>
#include <sstream>

namespace sw::game
{
	template <typename TCommand>
	void registerParserCommand(const std::shared_ptr<ecs::Context>& context, io::CommandParser& parser)
	{
		parser.add<TCommand>(
			[context](auto data)
			{
				printDebug(std::cout, data);

				context->getDispatcher() << data;
			});
	}

	template <typename... TCommand>
	void registerParserCommands(const std::shared_ptr<ecs::Context>& context, io::CommandParser& parser)
	{
		(registerParserCommand<TCommand>(context, parser), ...);
	}

	static bool hasAnyCommands(const std::shared_ptr<ecs::Context>& context)
	{
		debug::check(context, "invalid context");

		return !context->for_each<BehaviourComponent>(
			[](const uint32_t entityId, std::shared_ptr<BehaviourComponent> behaviour)
			{ return behaviour->targetId != InvalidId || behaviour->waypoint.has_value(); });
	}

	GameApp::GameApp() :
			context(std::make_shared<ecs::Context>())
	{
		init();
	}

	const std::shared_ptr<ecs::Context>& GameApp::getContext() const
	{
		return context;
	}

	void GameApp::exec(const uint32_t tickLimit)
	{
		debug::check(context, "invalid context");

		while (!hasAnyCommands(context))
		{
			context->advance();

			if (context->getTickCount() >= tickLimit)
			{
				std::cout << "Simulation interupted: Reached tick limit\n";
				break;
			}
		}

		std::cout << "Simulation ended on " << context->getTickCount() << " turn\n";
	}

	void GameApp::clear()
	{
		debug::check(context, "invalid context");

		context->clear();
	}

	bool GameApp::loadFromStream(std::istream& stream)
	{
		debug::check(context, "invalid context");

		if (!stream)
		{
			return false;
		}

		clear();

		// eval game scenario
		std::cout << "Commands:\n";
		io::CommandParser parser;
		registerParserCommands<
			io::CreateMap,
			io::SpawnSwordsman,
			io::SpawnHunter,
			io::SpawnTower,
			io::SpawnHealer,
			io::March>(context, parser);
		parser.parse(stream);

		// force dispatch events
		context->getDispatcher().dispatchAll();
		return true;
	}

	bool GameApp::loadFromFile(const std::string& fileName)
	{
		std::ifstream file(fileName);
		return loadFromStream(file);
	}

	bool GameApp::loadFromStr(const std::string& scenario)
	{
		std::stringstream stream(scenario);
		return loadFromStream(stream);
	}

	void GameApp::init()
	{
		debug::check(context, "invalid context");

		// init systems (order important)
		// poi: introduce system dependency resolving (DAG)
		context->addSystem(std::make_unique<game::SpawnUnitSystem>(context));
		context->addSystem(std::make_unique<game::VisibilitySystem>(context));
		context->addSystem(std::make_unique<game::AISystem>(context));
		context->addSystem(std::make_unique<game::DamageSystem>(context));
		context->addSystem(std::make_unique<game::MovementSystem>(context));
		context->addSystem(std::make_unique<game::LoggerSystem>(context));
	}
}
