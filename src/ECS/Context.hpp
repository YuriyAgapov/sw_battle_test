#pragma once

#include "Component.hpp"
#include "ECS/EventDispatcher.hpp"
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
		using ComponentMap = std::unordered_map<uint64_t, std::shared_ptr<Component>>;

		virtual ~Context();

		Entity& addEntity();

		template <typename ... ComponentType>
		Entity& addEntity()
		{
			auto& entity = addEntity();
			(addComponent<ComponentType>(entity),...);
			return entity;
		}

		const Entity& getEntity(const uint32_t entityId) const;

		void clear();

		void advance();

		template <typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> addComponent(Entity& entity, Args... args)
		{
			auto component = std::make_shared<ComponentType>(args...);
			component->id = makeComponentId<ComponentType>(entity.id);
			components.emplace(component->id, component);
			return component;
		}

		template <typename ComponentType>
		std::shared_ptr<ComponentType> getComponent(const uint32_t entityId) const
		{
			const uint64_t componentId = makeComponentId<ComponentType>(entityId);
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

		void addSystem(std::unique_ptr<System> system);

		const EntityMap& getEntities() const;
		const ComponentMap& getComponents() const;

		EventDispatcher& getDispatcher();

	private:
		static uint64_t makeComponentId(const uint32_t entityId, const size_t typeHash)
		{
			return static_cast<uint64_t>(entityId) << 32 | typeHash;
		}
		template <typename ComponentType>
		static uint64_t makeComponentId(const uint32_t entityId)
		{
			return makeComponentId(entityId, typeid(ComponentType).hash_code());
		}
		static bool IsOwner(const uint32_t entityId, const uint64_t componentId)
		{
			return entityId == (componentId >> 32);
		}

		EntityMap entities;
		ComponentMap components;
		std::vector<std::unique_ptr<System>> systems;
		EventDispatcher eventDispatcher;

		uint32_t nextEntityId = 0;
	};
}
