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
#include <Game/Systems/WeaponSystem.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/PrintDebug.hpp>
#include <Math/Rect2.hpp>
#include <fstream>

namespace sw::game
{
	static bool hasAnyCommands(const std::shared_ptr<ecs::Context>& context)
	{
		debug::check(context, "invalid context");

		return !context->for_each<WeaponComponent, BehaviourComponent>(
			[](const uint32_t entityId, auto weaponComponent, std::shared_ptr<BehaviourComponent> behaviour)
			{
				for (auto& [_, weapon] : weaponComponent->weapons)
				{
					if (weapon.targetId != InvalidId)
					{
						return false;
					}
				}

				return behaviour->target.has_value();
			});
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

	bool GameApp::loadFromFile(const std::string& fileName)
	{
		debug::check(context, "invalid context");

		std::ifstream file(fileName);
		if (!file)
		{
			return false;
		}

		// eval game scenario
		std::cout << "Commands:\n";
		io::CommandParser parser;
		parser
			.add<io::CreateMap>(
				[this](auto data)
				{
					printDebug(std::cout, data);

					// setup world
					context->getDispatcher() << data;
				})
			.add<io::SpawnSwordsman>(
				[this](auto data)
				{
					printDebug(std::cout, data);

					context->getDispatcher() << data;
				})
			.add<io::SpawnHunter>(
				[this](auto data)
				{
					printDebug(std::cout, data);

					context->getDispatcher() << data;
				})
			.add<io::March>(
				[this](auto data)
				{
					printDebug(std::cout, data);

					context->getDispatcher() << data;
				});

		parser.parse(file);

		// force dispatch events
		context->getDispatcher().dispatchAll();
		return true;
	}

	void GameApp::init()
	{
		debug::check(context, "invalid context");

		// init singletone first
		context->addSingletoneComponent<game::GridComponent>();

		// init systems (order important)
		// poi: introduce system dependency resolving (DAG)
		context->addSystem(std::make_unique<game::VisibilitySystem>(context));
		context->addSystem(std::make_unique<game::SpawnUnitSystem>(context));
		context->addSystem(std::make_unique<game::AISystem>(context));
		context->addSystem(std::make_unique<game::WeaponSystem>(context));
		context->addSystem(std::make_unique<game::DamageSystem>(context));
		context->addSystem(std::make_unique<game::MovementSystem>(context));
		context->addSystem(std::make_unique<game::LoggerSystem>(context));
	}
}
