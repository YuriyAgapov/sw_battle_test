#pragma once

#include <memory>

namespace sw
{
	namespace ecs { class Context; }
	namespace math { struct Vector2; }
}

namespace sw::debug
{
	template <typename T>
	inline void check(T expr, const std::string& msg)
	{
		//todo: disable with conditional compilation
		if (!expr)
			throw std::runtime_error(msg);
	}

	void checkPosition(const std::shared_ptr<ecs::Context>& context, const math::Vector2& pos);
}
