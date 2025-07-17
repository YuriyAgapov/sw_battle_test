#include "Attributes.hpp"

namespace sw::scheme
{
	const char* getAttributeName(const AttributeType Id)
	{
		switch (Id)
		{
			case AttributeType::UnitId: return "unitId";
			case AttributeType::X: return "x";
			case AttributeType::Y: return "y";
			case AttributeType::HP: return "hp";
			case AttributeType::Agility: return "agility";
			case AttributeType::Strength: return "strength";
			case AttributeType::Range: return "range";
			case AttributeType::AttackType: return "attackType";
			case AttributeType::Blocking: return "blocking";
		}
		return "Unknown";
	}
}
