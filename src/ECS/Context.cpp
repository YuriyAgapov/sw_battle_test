#include "Context.hpp"
#include "System.hpp"

namespace sw::ecs
{
	Context::~Context()
	{
	}

	Entity& Context::addEntity()
	{
		++nextEntityId;
		auto iter = entities.emplace(nextEntityId, Entity{nextEntityId, {}});
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
		nextEntityId = 0;
		nextComponentId = 0;
	}

	void Context::advance()
	{
		for (auto& system : systems)
		{
			system->advance();
		}

		std::erase_if(
			entities,
			[this](const auto& item)
			{
				auto& entity = item.second;
				if (entity.deleteLater)
				{
					for (auto comp_iter : entity.components)
					{
						components.erase(comp_iter.second);
					}
				}
				return entity.deleteLater;
			});
	}

	void Context::addSystem(std::unique_ptr<System> system)
	{
		systems.emplace_back(std::move(system));
	}
}
