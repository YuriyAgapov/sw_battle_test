#include <ECS/Context.hpp>
#include <ECS/System.hpp>
#include <Game/Components/UnitComponent.hpp>
#include <Game/Components/UnitMovementComponent.hpp>
#include <gtest/gtest.h>

using namespace sw;

class EcsTest : public testing::Test
{
protected:
	void SetUp() override
	{
		context = std::make_shared<ecs::Context>();
	}

	void TearDown() override
	{
		context.reset();
	}

	std::shared_ptr<ecs::Context> context;
};

TEST_F(EcsTest, AddEntity)
{
	auto& entity = context->addEntity();
	auto comp = context->addComponent<game::UnitComponent>(entity);
	EXPECT_EQ(entity.id, comp->entityId);
	EXPECT_EQ(entity.components.size(), 1);
}

TEST_F(EcsTest, getComponent)
{
	auto& entity = context->addEntity();
	auto addedComp = context->addComponent<game::UnitComponent>(entity);
	auto foundComp = context->getComponent<game::UnitComponent>(entity.id);
	EXPECT_EQ(foundComp, addedComp);
}

TEST_F(EcsTest, getComponents)
{
	auto& entity = context->addEntity();
	auto comp1 = context->addComponent<game::UnitComponent>(entity);
	auto comp2 = context->addComponent<game::UnitMovementComponent>(entity);

	auto [foundA, foundB] = context->getComponents<game::UnitComponent, game::UnitMovementComponent>(entity.id);

	EXPECT_EQ(comp1, foundA);
	EXPECT_EQ(comp2, foundB);
}

TEST_F(EcsTest, for_each)
{
	auto& entity = context->addEntity();
	context->addComponent<game::UnitComponent>(entity);

	std::vector<std::shared_ptr<game::UnitComponent>> coms;
	context->for_each<game::UnitComponent>(
		[&coms](auto comp)
		{
			//...
			coms.push_back(comp);
			return true;
		});
}
