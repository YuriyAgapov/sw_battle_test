#include <Math/Vector2.hpp>

#include <gtest/gtest.h>

using namespace sw;

using MakeVelocity_TestParam = std::tuple<math::Vector2, math::Vector2, uint32_t, math::Vector2>;

class MakeVelocityTest : public ::testing::TestWithParam<MakeVelocity_TestParam>
{};

TEST_P(MakeVelocityTest, ReturnsExpectedResult)
{
	const auto& [start, end, dist, expected] = GetParam();
	EXPECT_EQ(math::makeVelocity(start, end, dist), expected);
}

INSTANTIATE_TEST_SUITE_P(
	MoveToCases,
	MakeVelocityTest,
	::testing::Values(
		MakeVelocity_TestParam{{1, 1}, {2, 2}, 0, {0, 0}},  // stay
		MakeVelocity_TestParam{{1, 1}, {1, 1}, 1, {0, 0}},  // stay
		MakeVelocity_TestParam{{1, 1}, {1, 2}, 1, {0, 1}},  // top
		MakeVelocity_TestParam{{2, 2}, {2, 1}, 1, {0, -1}},  // bottom
		MakeVelocity_TestParam{{1, 1}, {2, 1}, 1, {1, 0}},  // right
		MakeVelocity_TestParam{{2, 2}, {1, 2}, 1, {-1, 0}},  // left
		MakeVelocity_TestParam{{1, 1}, {2, 2}, 1, {1, 1}},  // right top
		MakeVelocity_TestParam{{1, 1}, {2, 0}, 1, {1, -1}},  // right bottom
		MakeVelocity_TestParam{{1, 1}, {0, 2}, 1, {-1, 1}},  // left top
		MakeVelocity_TestParam{{1, 1}, {0, 0}, 1, {-1, -1}},  // left bottom
		MakeVelocity_TestParam{{1, 1}, {2, 4}, 2, {1, 2}}	  // long jump
		));
