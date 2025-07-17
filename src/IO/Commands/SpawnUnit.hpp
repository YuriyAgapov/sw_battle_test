#pragma once

#include "CommandTraits.hpp"

#include "Scheme/Attributes.hpp"
#include "Scheme/Units.hpp"

#include <stdexcept>

namespace sw::io
{
	template <scheme::UnitType TUnitType>
	struct SpawnUnit
	{
		static constexpr scheme::UnitType type = TUnitType;
		scheme::AttributeValues attributes;
		scheme::AbilitySet abilities;

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			// init default attributes
			const auto unitPrototype = scheme::getUnits().find(type);
			if (unitPrototype == scheme::getUnits().end())
			{
				throw std::runtime_error("Prototype for the unit not found");
			}
			attributes = unitPrototype->second.defaultAttributes;

			// setup unit from command by protocol
			for (const scheme::AttributeType attribute : unitPrototype->second.configAttributes)
			{
				visitor.visit(scheme::getAttributeName(attribute), attributes[attribute]);
			}

			// setup abilities
			abilities = unitPrototype->second.defaultAbilities;
		}
	};

	template<scheme::UnitType TUnitType>
	struct CommandTraits< SpawnUnit<TUnitType> >
	{
		static constexpr const char* getName()
		{
			switch (TUnitType)
			{
				case scheme::UnitType::Swordsman: return "SPAWN_SWORDSMAN";
				case scheme::UnitType::Hunter: return "SPAWN_HUNTER";
			}
			return "Unknown";
		}
	};
}
