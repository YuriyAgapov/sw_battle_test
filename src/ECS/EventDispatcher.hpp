#pragma once

#include <functional>
#include <memory>
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
			//impr: collapse similar events
			getChannel<TEventType>().events.emplace_back(std::move(event));
		}

		template <typename TEventType>
		void subscribe(Handler<TEventType>&& handler)
		{
			getChannel<TEventType>().handlers.emplace_back(handler);
		}

		void dispatchAll()
		{
			for (const auto& dispatcher : dispatchers)
			{
				dispatcher();
			}
		}

		template <typename TEventType>
		EventDispatcher& operator<<(TEventType event)
		{
			enqueueEvent(std::move(event));
			return *this;
		}

	private:
		template <typename TEventType>
		struct Channel
		{
			std::vector<TEventType> events;
			std::vector<Handler<TEventType>> handlers;
		};

		std::unordered_map<std::type_index, std::shared_ptr<void>> channels;
		std::vector<std::function<void()>> dispatchers;

		template <typename TEventType>
		Channel<TEventType>& getChannel()
		{
			std::shared_ptr<void>& channelPtr = channels[std::type_index(typeid(TEventType))];
			if (!channelPtr)
			{
				auto channel = std::make_shared<Channel<TEventType>>();
				dispatchers.emplace_back(
					[channel]()
					{
						// move events, any events generated while current slice processing will be delayed to the next tick
						std::vector<TEventType> events = std::move(channel->events);
						for (const auto& event : events)
						{
							for (const auto& handler : channel->handlers)
							{
								handler(event);
							}
						}
					});
				channelPtr = channel;
			}
			return *std::static_pointer_cast<Channel<TEventType>>(channelPtr);
		}
	};
}
