#pragma once

#include "Component.hpp"
#include "ECS/EventDispatcher.hpp"
#include "ComponentIndex.hpp"
#include "Entity.hpp"

#include <assert.h>
#include <memory>
#include <unordered_map>
#include <vector>

namespace sw::ecs
{
	class System;

	class Context
	{
	public:
		using EntityMap = std::unordered_map<uint32_t, Entity>;
		using ComponentMap = std::unordered_map<ComponentIndex, std::shared_ptr<Component>>;

		virtual ~Context();

		Entity& addEntity(const uint32_t entityId);

		template <typename ... ComponentType>
		Entity& addEntity(const uint32_t entityId)
		{
			auto& entity = addEntity(entityId);
			(addComponent<ComponentType>(entity),...);
			return entity;
		}

		const Entity& getEntity(const uint32_t entityId) const;

		void clear();

		void advance();

		void addSystem(std::unique_ptr<System> system);

		const EntityMap& getEntities() const;
		const ComponentMap& getComponents() const;

		EventDispatcher& getDispatcher();

		uint32_t getTickCount() const;

		template <typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> addComponent(Entity& entity, Args... args)
		{
			auto component = std::make_shared<ComponentType>(args...);
			component->id = makeIndex<ComponentType>(entity.id);
			components.emplace(component->id, component);
			return component;
		}

		template <typename ComponentType>
		std::shared_ptr<ComponentType> getComponent(const uint32_t entityId) const
		{
			const ComponentIndex componentId = makeIndex<ComponentType>(entityId);
			const auto compIter = components.find(componentId);
			return compIter != components.end() ? std::static_pointer_cast<ComponentType>(compIter->second) : nullptr;
		}

		template <typename... ComponentTypes>
		std::tuple<std::shared_ptr<ComponentTypes>...> getComponents(const uint32_t entityId) const
		{
			return std::make_tuple(getComponent<ComponentTypes>(entityId)...);
		}

		template <typename... ComponentTypes, typename Function>
		bool for_each(Function&& function)
		{
			for (const auto& [entityId, entity] : entities)
			{
				// collect components by type
				auto componentsTuple = getComponents<ComponentTypes...>(entityId);

				// check compenents consistensy
				if ((... && std::get<std::shared_ptr<ComponentTypes>>(componentsTuple)))
				{
					// prepend entity as first arg
					auto args = std::tuple_cat(std::tie(entity), componentsTuple);

					// call handler
					if (!std::apply(function, args))
					{
						return false;
					}
				}
			}
			return true;
		}

	private:
		static ComponentIndex makeIndex(const uint32_t entityId, const size_t typeId)
		{
			return ComponentIndex{entityId, typeId};
		}
		template <typename ComponentType>
		static ComponentIndex makeIndex(const uint32_t entityId)
		{
			return makeIndex(entityId, typeid(ComponentType).hash_code());
		}
		static bool IsOwner(const uint32_t entityId, const ComponentIndex& index)
		{
			return entityId == index.enitityId;
		}

		EntityMap entities;
		ComponentMap components;
		std::vector<std::unique_ptr<System>> systems;
		EventDispatcher eventDispatcher;
		uint32_t tickCount = 0;
	};
}
