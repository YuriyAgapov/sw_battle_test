#include "Units.hpp"

#include "Attributes.hpp"
#include "Scheme/AttackType.hpp"

namespace sw::scheme
{

	const std::unordered_map<UnitType, UnitScheme>& getUnits()
	{
		//todo: read from ... e.g. json
		// clang-format off
		static std::unordered_map<UnitType, UnitScheme> units = {
			{
				UnitType::Swordsman,
				UnitScheme{
					"Swordsman",
					{{AttributeType::AttackType, uint32_t(AttackType::Melee)}, {AttributeType::Blocking, 1}},
					{AttributeType::UnitId, AttributeType::X, AttributeType::Y, AttributeType::HP, AttributeType::Strength}
				}
			},
			{
				UnitType::Hunter,
				UnitScheme{
					"Hunter",
					{{AttributeType::AttackType, uint32_t(AttackType::Range)}, {AttributeType::Blocking, 1}},
					{AttributeType::UnitId, AttributeType::X, AttributeType::Y, AttributeType::HP, AttributeType::Agility, AttributeType::Strength, AttributeType::Range}
				}
			}
		};
		// clang-format on
		return units;
	}

	bool isValidAttributes(const UnitType type, const AttributeValues& values)
	{
		const auto iter = getUnits().find(type);
		if (iter == getUnits().end())
		{
			return false;
		}

		for (const AttributeType id : iter->second.configAttributes)
		{
			if (!values.contains(id))
			{
				return false;
			}
		}
		return true;
	}

}
