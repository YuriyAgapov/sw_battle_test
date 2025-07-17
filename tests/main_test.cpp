#include <Math/Vector2D.hpp>

#include <gtest/gtest.h>

using namespace sw;

TEST(MathTest, SubOper)
{
	const math::Vector2u a{5, 7};
	const math::Vector2u b{2, 3};
	const math::Vector2u diff = a - b;
	EXPECT_EQ(diff.x, 3);
	EXPECT_EQ(diff.y, 4);
}

