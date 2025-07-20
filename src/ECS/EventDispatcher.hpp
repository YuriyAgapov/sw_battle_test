#pragma once

#include <any>
#include <deque>
#include <functional>
#include <typeindex>

namespace sw::ecs
{
	class EventDispatcher
	{
	public:
		template <typename TEventType>
		using Handler = std::function<void(const TEventType& event)>;

		template <typename TEventType>
		void enqueueEvent(TEventType event)
		{
			events.emplace_back(std::move(event));
		}

		template <typename TEventType>
		void subscribe(Handler<TEventType>&& handler)
		{
			handlers[typeid(TEventType)].emplace_back(
				[handler = handler](const std::any& event)
				{
					// better to use rtti
					handler(std::any_cast<const TEventType&>(event));
				});
		}

		void dispatchAll()
		{
			size_t i = 0;
			while (i < events.size())
			{
				const auto& event = events[i];
				auto iter = handlers.find(event.type());
				if (iter != handlers.end())
				{
					for (const auto& handler : iter->second)
					{
						handler(event);
					}
				}
				++i;
			}
			events.clear();
		}

		template <typename TEventType>
		EventDispatcher& operator<<(TEventType event)
		{
			enqueueEvent(std::move(event));
			return *this;
		}

	private:
		std::unordered_map<std::type_index, std::vector<std::function<void(const std::any&)>>> handlers;
		std::deque<std::any> events;
	};
}
