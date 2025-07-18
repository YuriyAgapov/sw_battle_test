#include "Context.hpp"

#include "System.hpp"

namespace sw::ecs
{
	Context::~Context() {}

	Entity& Context::addEntity(const uint32_t entityId)
	{
		auto iter = entities.emplace(entityId, Entity{entityId, {}});
		assert(iter.second);

		return iter.first->second;
	}

	const Entity& Context::getEntity(const uint32_t entityId) const
	{
		auto iter = entities.find(entityId);
		assert(iter != entities.end());
		return iter->second;
	}

	void Context::clear()
	{
		entities.clear();
		components.clear();
		systems.clear();
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
		std::erase_if(
			entities,
			[this](const auto& entityItem)
			{
				const Entity& entity = entityItem.second;
				if (entity.deleteLater)
				{
					// remove relatecd components
					std::erase_if(
						components,
						[entityId = entity.id](const auto& compItem)
						{
							return IsOwner(entityId, compItem.first);
						}
					);
				}
				return entity.deleteLater;
			});

		++tickCount;
	}

	void Context::addSystem(std::unique_ptr<System> system)
	{
		systems.emplace_back(std::move(system));
	}

	const std::unordered_map<uint32_t, Entity>& Context::getEntities() const
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
