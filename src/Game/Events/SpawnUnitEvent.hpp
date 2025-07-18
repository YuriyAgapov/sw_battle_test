#pragma once

#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>

namespace sw::game
{
	struct SpawnSwordsmanUnitEvent
	{
		io::SpawnSwordsman data;
	};

	struct SpawnHunterUnitEvent
	{
		io::SpawnHunter data;
	};
}
