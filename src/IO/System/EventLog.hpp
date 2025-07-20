#pragma once

#include "details/PrintFieldVisitor.hpp"
#include <cstdint>
#include <iostream>

namespace sw
{
	class EventLog
	{
	public:
		template <class TEvent>
		static void log(uint64_t tick, TEvent&& event)
		{
			std::cout << "[" << tick << "] " << std::remove_reference_t<TEvent>::Name << " ";
			PrintFieldVisitor visitor(std::cout);
			event.visit(visitor);
			std::cout << std::endl;
		}
	};
}
