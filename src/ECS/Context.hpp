#pragma once

#include "Component.hpp"
#include "Entity.hpp"

#include <assert.h>
#include <memory>
#include <vector>

namespace sw::ecs
{
	class System;

	class Context
	{
	public:
		virtual ~Context();

		Entity& addEntity();

		const Entity& getEntity(const uint32_t entityId) const;

		void clear();

		void advance();

		template <typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> addComponent(Entity& entity, Args... args)
		{
			++nextComponentId;
			auto iter = components.emplace(nextComponentId, std::make_shared<ComponentType>(args...));
			assert(iter.second);
			std::shared_ptr<Component>& component = iter.first->second;
			component->componentId = nextComponentId;
			component->entityId = entity.id;
			entity.components.emplace(typeid(ComponentType).hash_code(), component->componentId);
			return std::static_pointer_cast<ComponentType>(component);
		}

		template <typename ComponentType>
		std::shared_ptr<ComponentType> getComponent(const uint32_t entityId) const
		{
			const Entity& entity = getEntity(entityId);
			auto typeIter = entity.components.find(typeid(ComponentType).hash_code());
			if (typeIter == entity.components.end())
			{
				return nullptr;
			}

			auto compIter = components.find(typeIter->second);
			if (compIter == components.end())
			{
				return nullptr;
			}

			return std::static_pointer_cast<ComponentType>(compIter->second);
		}

		template <typename... ComponentTypes>
		std::tuple<std::shared_ptr<ComponentTypes>...> getComponents(uint32_t entityId) const
		{
			return std::make_tuple(getComponent<ComponentTypes>(entityId)...);
		}

		// template <typename ComponentType, typename Function>
		// bool for_each(Function function)
		// {
		// 	for (const auto& [id, entity] : entities)
		// 	{
		// 		if (auto component = getComponent<ComponentType>(id))
		// 			if (!function(component))
		// 				return false;
		// 	}
		// 	return true;
		// }

		template <typename... ComponentTypes, typename Function>
		bool for_each(Function&& function)
		{
			for (const auto& [entityId, entity] : entities)
			{
				//auto componentsTuple = std::make_tuple(getComponents<ComponentTypes>(entityId)...);
				auto componentsTuple = getComponents<ComponentTypes...>(entityId);

				if ((... && std::get<std::shared_ptr<ComponentTypes>>(componentsTuple)))
				{
					if (!std::apply(function, componentsTuple))
					{
						return false;
					}
				}
			}
			return true;
		}

		void addSystem(std::unique_ptr<System> system);

	private:
		std::unordered_map<uint32_t, Entity> entities;
		std::unordered_map<uint32_t, std::shared_ptr<Component>> components;
		std::vector<std::unique_ptr<System>> systems;

		uint32_t nextEntityId = 0;
		uint32_t nextComponentId = 0;
	};
}
