#include "LoggerSystem.hpp"

#include <ECS/Context.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitHealed.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/EventLog.hpp>

namespace sw::game
{
	template <typename TEvent>
	static void subscribeLogger(const std::shared_ptr<ecs::Context>& context)
	{
		context->getDispatcher().subscribe<TEvent>(
			[context](const TEvent& event)
			{
				EventLog::log(context->getTickCount(), event);
			});
	}

	LoggerSystem::LoggerSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{
		subscribeLogger<io::MapCreated>(context);
		subscribeLogger<io::MarchStarted>(context);
		subscribeLogger<io::MarchEnded>(context);
		subscribeLogger<io::UnitSpawned>(context);
		subscribeLogger<io::UnitDied>(context);
		subscribeLogger<io::UnitAttacked>(context);
		subscribeLogger<io::UnitHealed>(context);
		subscribeLogger<io::UnitMoved>(context);
	}

	void LoggerSystem::advance()
	{
		//nothing
	}

}
