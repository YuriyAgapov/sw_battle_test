#pragma once

#include "details/PrintFieldVisitor.hpp"
#include "IO/Commands/CommandTraits.hpp"

namespace sw
{
	template <typename TCommand>
	void printDebug(std::ostream& stream, TCommand& data)
	{
		stream << io::CommandTraits<TCommand>::getName() << ' ';
		PrintFieldVisitor visitor(stream);
		data.visit(visitor);
		stream << "\n";
	}
}
