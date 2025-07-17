#pragma once

#include <cmath>
#include <cstdint>

namespace sw::math
{
	template <typename TValue>
	bool isEqual(const TValue a, const TValue b, const TValue tolerance = std::numeric_limits<TValue>::epsilon())
	{
		if constexpr (std::is_floating_point_v<TValue>)
			return std::abs(a - b) < tolerance;
		else
			return a == b;
	}

	template <typename TTo, typename TFrom>
	TTo roundValue(const TFrom value)
	{
		if constexpr (std::is_floating_point_v<TFrom>)
			return static_cast<TTo>(std::round(value));
		else
			return static_cast<TTo>(value);
	}

	template <typename TValue>
	struct Vector2
	{
		using ValueType = TValue;

		TValue x{};
		TValue y{};

		bool operator == (const Vector2<TValue>& other) const
		{
			return isEqual(x, other.x) && isEqual(y, other.y);
		}

		template <typename TToVector>
		TToVector to() const
		{
			return TToVector{roundValue<typename TToVector::ValueType>(x), roundValue<typename TToVector::ValueType>(y)};
		}
	};

	using Vector2d = Vector2<double>;
	using Vector2u = Vector2<uint32_t>;

	template <typename TToVector, typename TFromVector>
	TToVector round(const TFromVector& vector)
	{
		return vector.template to<TToVector>();
	}

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
}
