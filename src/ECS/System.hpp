#pragma once


namespace sw::ecs
{
	struct System
	{
		virtual ~System() = default;

		virtual void advance() = 0;
	};
}
