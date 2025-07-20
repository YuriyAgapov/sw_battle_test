#include "Game/Types/EntityMapping.hpp"

#include <Math/Vector2.hpp>
#include <gtest/gtest.h>

using namespace sw;

TEST(EntityMappingTest, add)
{
	game::EntityMapping mapping;
	mapping.add(1, math::Vector2{1, 2});

	EXPECT_EQ(mapping.get(math::Vector2{1, 2}), (std::unordered_set<uint32_t>{1}));
}

TEST(EntityMappingTest, addMultiple)
{
	game::EntityMapping mapping;
	mapping.add(1, math::Vector2{1, 2});
	mapping.add(2, math::Vector2{1, 2});

	EXPECT_EQ(mapping.get(math::Vector2{1, 2}), (std::unordered_set<uint32_t>{1, 2}));
}

TEST(EntityMappingTest, remove)
{
	game::EntityMapping mapping;
	mapping.add(1, math::Vector2{1, 2});
	mapping.remove(1, math::Vector2{1, 2});

	EXPECT_TRUE(mapping.get(math::Vector2{1, 2}).empty());
}

TEST(EntityMappingTest, removeFromSet)
{
	game::EntityMapping mapping;
	mapping.add(1, math::Vector2{1, 2});
	mapping.add(2, math::Vector2{1, 2});
	mapping.remove(1, math::Vector2{1, 2});

	EXPECT_EQ(mapping.get(math::Vector2{1, 2}), (std::unordered_set<uint32_t>{2}));
}
