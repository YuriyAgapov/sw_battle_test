#include "Math/Algo.hpp"

#include <Math/Rect2.hpp>
#include <Math/Vector2.hpp>
#include <gtest/gtest.h>

using namespace sw;

// vec

TEST(MathTest, vecSubOper)
{
	const math::Vector2 a{5, 7};
	const math::Vector2 b{2, 3};
	const math::Vector2 diff = a - b;
	EXPECT_EQ(diff.x, 3);
	EXPECT_EQ(diff.y, 4);
}

TEST(MathTest, vecAddOper)
{
	const math::Vector2 a{5, 7};
	const math::Vector2 b{2, 3};
	const math::Vector2 diff = a + b;
	EXPECT_EQ(diff.x, 7);
	EXPECT_EQ(diff.y, 10);
}

TEST(MathTest, vecEqOper)
{
	const math::Vector2 a{5, 7};
	const math::Vector2 b{7, 5};
	const math::Vector2 c{5, 7};
	EXPECT_NE(a, b);
	EXPECT_EQ(a, c);
}

TEST(MathTest, length)
{
	const math::Vector2 a{3, 4};
	EXPECT_EQ(math::length(a), 5);
}

TEST(MathTest, roundVec)
{
	math::Vector2 expected{2, 4};
	EXPECT_EQ(math::roundVector(2.2, 3.6), expected);
}

// rect

TEST(MathTest, rectContains)
{
	math::Rect2 rect{10, 10};
	EXPECT_TRUE(rect.contains(math::Vector2{0, 0}));
	EXPECT_TRUE(rect.contains(math::Vector2{10, 0}));
	EXPECT_TRUE(rect.contains(math::Vector2{0, 10}));
	EXPECT_TRUE(rect.contains(math::Vector2{5, 5}));
	EXPECT_TRUE(rect.contains(math::Vector2{10, 10}));
	EXPECT_FALSE(rect.contains(math::Vector2{11, 0}));
	EXPECT_FALSE(rect.contains(math::Vector2{0, 11}));
	EXPECT_FALSE(rect.contains(math::Vector2{11, 11}));
	EXPECT_FALSE(rect.contains(math::Vector2{-1, -1}));
}

// algo section

TEST(MathTest, foreachRect)
{
	std::vector<math::Vector2> actualPoints;
	actualPoints.reserve(9);
	math::foreachRect(
		{2, 3},
		1,
		[&actualPoints](const math::Vector2& point)
		{
			actualPoints.push_back(point);
			return true;
		});

	//clang-format off
	std::vector<math::Vector2> expectedPoints = {
		{1, 2}, {2, 2}, {3, 2},
		{1, 3}, {2, 3}, {3, 3},
		{1, 4}, {2, 4}, {3, 4},
	};
	//clang-format on
	EXPECT_EQ(actualPoints, expectedPoints);
}

TEST(MathTest, foreachCircle)
{
	std::vector<math::Vector2> actualPoints;
	actualPoints.reserve(9);
	math::foreachCircle(
		{2, 3},
		0,
		1,
		[&actualPoints](const math::Vector2& point)
		{
			actualPoints.push_back(point);
			return true;
		});

	//clang-format off
	std::vector<math::Vector2> expectedPoints = {
				{2, 2},
		{1, 3}, {2, 3}, {3, 3},
				{2, 4},
	};
	//clang-format on
	EXPECT_EQ(actualPoints, expectedPoints);
}

TEST(MathTest, foreachCircleMinMax)
{
	std::vector<math::Vector2> actualPoints;
	actualPoints.reserve(9);
	math::foreachCircle(
		{2, 2},
		1,
		2,
		[&actualPoints](const math::Vector2& point)
		{
			actualPoints.push_back(point);
			return true;
		});

	//clang-format off

	std::vector<math::Vector2> expectedPoints = {
						{2, 0},
				{1, 1}, {2, 1}, {3, 1},
		{0, 2}, {1, 2},         {3, 2}, {4, 2},
				{1, 3}, {2, 3}, {3, 3},
						{2, 4},
	};
	//clang-format on
	EXPECT_EQ(actualPoints, expectedPoints);
}
