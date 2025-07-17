#pragma once

#include <cmath>
#include <cstdint>

namespace sw::math
{
	template <typename TValue>
	struct Vector2
	{
		TValue x{};
		TValue y{};
	};

	using Vector2d = Vector2<double>;
	using Vector2u= Vector2<uint32_t>;

	template <typename TValue>
	inline Vector2<TValue> operator - (const Vector2<TValue>& a, const Vector2<TValue>& b)
	{
		return Vector2<TValue>{a.x - b.x, a.y - b.y};
	}

	template <typename TValue>
	inline Vector2<TValue> operator + (const Vector2<TValue>& a, const Vector2<TValue>& b)
	{
		return Vector2<TValue>{a.x + b.x, a.y + b.y};
	}

	template <typename TValue>
	inline Vector2<TValue> operator / (const Vector2<TValue>& a, const TValue value)
	{
		return Vector2<TValue>{a.x / value, a.y / value};
	}

	template <typename TValue>
	inline Vector2<TValue> operator * (const Vector2<TValue>& a, const TValue value)
	{
		return Vector2<TValue>{a.x * value, a.y * value};
	}

	template <typename TValue>
	inline TValue length(const Vector2<TValue>& vector)
	{
		return std::hypot(vector.x, vector.y);
	}

	inline Vector2d moveTo(const Vector2d& from, const Vector2d& to, const double distance)
	{
		const Vector2d delta = to - from;
		const double totalDistance = length(delta);
		if (totalDistance == 0.0f)
			return from;
		if (distance >= totalDistance)
			return to;

		const Vector2d direction = delta / totalDistance;
		return from + direction * distance;
	}

	template <typename TValue>
	inline Vector2u round(const Vector2<TValue>& vector)
	{
		return Vector2u{std::round(vector.x), std::round(vector.y)};
	}

	template <typename TValue>
	inline Vector2<TValue> operator == (const Vector2<TValue>& a, const Vector2<TValue>& b)
	{
		return a.x == b.x && a.y == b.y;
	}
}
