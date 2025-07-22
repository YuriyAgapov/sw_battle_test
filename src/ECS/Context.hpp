#pragma once

#include "EventDispatcher.hpp"
#include "ComponentIndex.hpp"

#include <assert.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <limits>
#include <unordered_set>

namespace sw::ecs
{
	class System;

	/** ECS system context, designed to isolate unrelated data, systems and events.
	 *  Responsible for storing entities, components and systems, is the entry point for accessing the event manager
	 **/
	class Context
	{
	public:
		static constexpr uint32_t SingletoneId = std::numeric_limits<uint32_t>::max();

		using EntitySet = std::unordered_set<uint32_t>;
		using ComponentMap = std::unordered_map<ComponentIndex, std::shared_ptr<void>>;

		virtual ~Context();

		/// returns true if the entity has been added, otherwise false if the entity is already registered
		bool addEntity(const uint32_t entityId);

		/**
		 *  returns true if the entity has been added, otherwise false if the entity is already registered
		 *  Сreates and associates components of the specified types with a new entity
		**/
		template <typename ... ComponentType>
		bool addEntity(const uint32_t entityId)
		{
			if (!addEntity(entityId))
				return false;
			(addComponent<ComponentType>(entityId),...);
			return true;
		}

		/// returns true if @arg entityId exists
		bool hasEntity(const uint32_t entityId) const;

		/// returns true if @arg entityId will be deleted after the current tick
		bool hasPendingKill(const uint32_t entityId) const;

		/// marks the specified entity @arg uint32_t for deletion at the end of a tick
		void removeEntity(const uint32_t entityId);

		/// removes entities, components and events
		void clear();

		/// Consistently advances all systems, then dispatches events, and then deletes entities awaiting deletion
		void advance();

		/// adds specified system to the context, for advance in tick
		void addSystem(std::unique_ptr<System> system);

		/// returns registered entities
		const EntitySet& getEntities() const;

		/// returns registered componenets
		const ComponentMap& getComponents() const;

		/// returns a reference to the current event dispatcher
		EventDispatcher& getDispatcher();

		/// returns current tick number
		uint32_t getTickCount() const;

		/// creates a component of the given type for the given entity
		template <typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> addComponent(const uint32_t entityId, Args... args)
		{
			auto component = std::make_shared<ComponentType>(args...);
			auto pair = components.emplace(makeIndex<ComponentType>(entityId), component);
			return pair.second ? component : nullptr;
		}

		/// creates a singletone component of the given type
		template <typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> addSingletoneComponent(Args... args)
		{
			return addComponent<ComponentType>(SingletoneId, args...);
		}

		/// returns a component of the given type if it was associated with the given entity
		template <typename ComponentType>
		std::shared_ptr<ComponentType> getComponent(const uint32_t entityId) const
		{
			const ComponentIndex componentId = makeIndex<ComponentType>(entityId);
			const auto compIter = components.find(componentId);
			return compIter != components.end() ? std::static_pointer_cast<ComponentType>(compIter->second) : nullptr;
		}

		template <typename ComponentType>
		std::shared_ptr<ComponentType> ensureComponent(const uint32_t entityId)
		{
			auto comp = getComponent<ComponentType>(entityId);
			return comp ? comp : addComponent<ComponentType>(entityId);
		}


		/// returns singletone component
		template <typename ComponentType>
		std::shared_ptr<ComponentType> getSingletoneComponent() const
		{
			return getComponent<ComponentType>(SingletoneId);
		}

		/// returns a tuple of components of the given type if they were associated with the given entity, may return nullptr as part of the tuple
		template <typename... ComponentTypes>
		std::tuple<std::shared_ptr<ComponentTypes>...> getComponents(const uint32_t entityId) const
		{
			return std::make_tuple(getComponent<ComponentTypes>(entityId)...);
		}

		/// iterates over all entities that satisfy the given archetype, if the handler returns - false - breaks the cycle
		template <typename... ComponentTypes, typename Function>
		bool for_each(Function&& function)
		{
			for (const uint32_t entityId : entities)
			{
				// collect components by type
				auto components = getComponents<ComponentTypes...>(entityId);

				// check compenents consistensy
				if ((... && std::get<std::shared_ptr<ComponentTypes>>(components)))
				{
					// call handler
					if (!std::apply(function, std::tuple_cat(std::tie(entityId), components)))
					{
						return false;
					}
				}
			}
			return true;
		}

		/// returns a list of component tuples for entities that match a given archetype
		template <typename... ComponentTypes>
		std::vector<std::tuple<uint32_t, std::shared_ptr<ComponentTypes>...>> makeView()
		{
			std::vector<std::tuple<uint32_t, std::shared_ptr<ComponentTypes>...>> view;
			for (const uint32_t entityId : entities)
			{
				// collect components by type
				auto components = getComponents<ComponentTypes...>(entityId);

				// check compenents consistensy
				if ((... && std::get<std::shared_ptr<ComponentTypes>>(components)))
				{
					view.emplace_back(std::tuple_cat(std::make_tuple(entityId), components));
				}
			}
			return view;
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

		// set of current entities
		EntitySet entities;

		// entities which will be deleted at the end of the tick
		EntitySet pendingKill;

		// current components
		ComponentMap components;

		// set of active systems
		std::vector<std::unique_ptr<System>> systems;

		// provides events handling
		EventDispatcher eventDispatcher;

		// tick counter, resets by clear
		uint32_t tickCount = 0;
	};
}
