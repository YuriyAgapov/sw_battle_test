#include "Context.hpp"

#include "System.hpp"

namespace sw::ecs
{
	Context::~Context() {}

	bool Context::addEntity(const uint32_t entityId)
	{
		auto pair = entities.emplace(entityId);
		return pair.second;
	}

	bool Context::hasEntity(const uint32_t entityId) const
	{
		return entities.contains(entityId);
	}

	void Context::removeEntity(const uint32_t entityId)
	{
		pendingKill.emplace(entityId);
	}

	void Context::clear()
	{
		entities.clear();
		components.clear();
		systems.clear();
		pendingKill.clear();
		tickCount = 0;
	}

	void Context::advance()
	{
		// advance systems
		for (auto& system : systems)
		{
			system->advance();
		}

		// prepare events
		eventDispatcher.dispatchAll();

		// remove pending kill
		for (const uint32_t entityId : pendingKill)
		{
			// remove relatecd components
			std::erase_if(
				components,
				[entityId](const auto& compItem)
				{
					// check component index
					return IsOwner(entityId, compItem.first);
				});
			entities.erase(entityId);
		}
		pendingKill.clear();
		++tickCount;
	}

	void Context::addSystem(std::unique_ptr<System> system)
	{
		systems.emplace_back(std::move(system));
	}

	const Context::EntitySet& Context::getEntities() const
	{
		return entities;
	}

	const Context::ComponentMap& Context::getComponents() const
	{
		return components;
	}

	EventDispatcher& Context::getDispatcher()
	{
		return eventDispatcher;
	}

	uint32_t Context::getTickCount() const
	{
		return tickCount;
	}
}
