#pragma once

#include <cmath>
#include <cstdint>
#include <functional>
#include <ostream>

namespace sw::math
{
	struct Vector2
	{
		// x, y in signed space
		int64_t x{0};
		int64_t y{0};

		uint32_t getX() const
		{
			return static_cast<uint32_t>(x);
		}
		uint32_t getY() const
		{
			return static_cast<uint32_t>(y);
		}

		bool operator == (const Vector2& other) const
		{
			return x == other.x && y == other.y;
		}

		std::size_t getHash() const
		{
			std::size_t h1 = std::hash<int64_t>{}(x);
			std::size_t h2 = std::hash<int64_t>{}(y);
			return h1 ^ (h2 << 1);
		}

		bool isZero() const
		{
			return x == 0 && y == 0;
		}

		//gtest
		friend void PrintTo(const sw::math::Vector2& v, std::ostream* os)
		{
			*os << "Vector2{" << v.x << ", " << v.y << "}";
		}
	};

	inline Vector2 operator - (const Vector2& a, const Vector2& b)
	{
		return Vector2{a.x - b.x, a.y - b.y};
	}

	inline Vector2 operator + (const Vector2& a, const Vector2& b)
	{
		return Vector2{a.x + b.x, a.y + b.y};
	}

	inline double length(const Vector2& vector)
	{
		return std::hypot(vector.x, vector.y);
	}

	inline Vector2 roundVector(const double x, const double y)
	{
		return {static_cast<int64_t>(round(x)), static_cast<int64_t>(round(y))};
	}

	inline Vector2 makeVelocity(const Vector2& from, const Vector2& to, const uint32_t speed)
	{
		if (speed == 0)
			return Vector2{};

		const Vector2 delta = to - from;
		const double totalDistance = length(delta);
		if (totalDistance == 0.0f)
			return Vector2{};
		if (speed >= totalDistance)
			return to - from;

		return roundVector(delta.x / totalDistance * speed, delta.y / totalDistance * speed);
	}
}

template<>
struct std::hash<sw::math::Vector2>
{
	std::size_t operator()(const sw::math::Vector2& vec) const noexcept
	{
		return vec.getHash();
	}
};
