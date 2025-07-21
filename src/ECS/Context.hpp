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

	class Context
	{
	public:
		static constexpr uint32_t SingletoneId = std::numeric_limits<uint32_t>::max();

		using EntitySet = std::unordered_set<uint32_t>;
		using ComponentMap = std::unordered_map<ComponentIndex, std::shared_ptr<void>>;

		virtual ~Context();

		bool addEntity(const uint32_t entityId);

		template <typename ... ComponentType>
		bool addEntity(const uint32_t entityId)
		{
			if (!addEntity(entityId))
				return false;
			(addComponent<ComponentType>(entityId),...);
			return true;
		}

		bool hasEntity(const uint32_t entityId) const;
		bool hasPendingKill(const uint32_t entityId) const;

		void removeEntity(const uint32_t entityId);

		void clear();

		void advance();

		void addSystem(std::unique_ptr<System> system);

		const EntitySet& getEntities() const;
		const ComponentMap& getComponents() const;

		EventDispatcher& getDispatcher();

		uint32_t getTickCount() const;

		template <typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> addComponent(const uint32_t entityId, Args... args)
		{
			auto component = std::make_shared<ComponentType>(args...);
			auto pair = components.emplace(makeIndex<ComponentType>(entityId), component);
			return pair.second ? component : nullptr;
		}

		template <typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> addSingletoneComponent(Args... args)
		{
			return addComponent<ComponentType>(SingletoneId, args...);
		}

		template <typename ComponentType>
		std::shared_ptr<ComponentType> getComponent(const uint32_t entityId) const
		{
			const ComponentIndex componentId = makeIndex<ComponentType>(entityId);
			const auto compIter = components.find(componentId);
			return compIter != components.end() ? std::static_pointer_cast<ComponentType>(compIter->second) : nullptr;
		}

		// returns singletone component
		template <typename ComponentType>
		std::shared_ptr<ComponentType> getSingletoneComponent() const
		{
			return getComponent<ComponentType>(SingletoneId);
		}

		template <typename... ComponentTypes>
		std::tuple<std::shared_ptr<ComponentTypes>...> getComponents(const uint32_t entityId) const
		{
			return std::make_tuple(getComponent<ComponentTypes>(entityId)...);
		}

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

		EntitySet entities;
		EntitySet pendingKill;
		ComponentMap components;
		std::vector<std::unique_ptr<System>> systems;
		EventDispatcher eventDispatcher;
		uint32_t tickCount = 0;
	};
}
