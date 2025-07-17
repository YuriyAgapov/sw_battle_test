#include <ECS/Context.hpp>
#include <ECS/System.hpp>
#include <gtest/gtest.h>

using namespace sw;

class TestCompA : public ecs::Component
{
};

class TestCompB : public ecs::Component
{
};

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
	auto comp = context->addComponent<TestCompA>(entity);
	EXPECT_TRUE(context->getEntities().contains(entity.id));
	EXPECT_TRUE(context->getComponents().contains(comp->id));
}

TEST_F(EcsTest, RemoveEntity)
{
	auto& entity = context->addEntity();
	context->advance();
	EXPECT_TRUE(context->getEntities().contains(entity.id));
	entity.deleteLater = true;
	context->advance();
	EXPECT_FALSE(context->getEntities().contains(entity.id));
}

TEST_F(EcsTest, getComponent)
{
	auto& entity = context->addEntity();
	auto addedComp = context->addComponent<TestCompA>(entity);
	auto foundComp = context->getComponent<TestCompA>(entity.id);
	EXPECT_EQ(foundComp, addedComp);
}

TEST_F(EcsTest, getComponents)
{
	auto& entity = context->addEntity();
	auto comp1 = context->addComponent<TestCompA>(entity);
	auto comp2 = context->addComponent<TestCompB>(entity);

	auto [foundA, foundB] = context->getComponents<TestCompA, TestCompB>(entity.id);

	EXPECT_EQ(comp1, foundA);
	EXPECT_EQ(comp2, foundB);
}

TEST_F(EcsTest, forEachSimple)
{
	auto& entity = context->addEntity();
	auto comp = context->addComponent<TestCompA>(entity);

	std::vector<std::shared_ptr<TestCompA>> comps;
	context->for_each<TestCompA>(
		[&comps](auto entity, auto comp)
		{
			//...
			comps.push_back(comp);
			return true;
		});
	EXPECT_EQ(comps.size(), 1);
	EXPECT_EQ(comps[0], comp);
}

TEST_F(EcsTest, forEachSeveral)
{
	context->addEntity<TestCompA>();
	context->addEntity<TestCompA, TestCompB>();
	context->addEntity<TestCompB>();

	uint32_t aCount = 0;
	context->for_each<TestCompA>(
		[&aCount](auto entity, auto comp)
		{
			++aCount;
			return true;
		});
	uint32_t bCount = 0;
	context->for_each<TestCompB>(
		[&bCount](auto entity, auto comp)
		{
			++bCount;
			return true;
		});
	uint32_t abCount = 0;
	context->for_each<TestCompA, TestCompB>(
		[&abCount](auto entity, auto compA, auto compB)
		{
			++abCount;
			return true;
		});
	EXPECT_EQ(aCount, 2);
	EXPECT_EQ(bCount, 2);
	EXPECT_EQ(abCount, 1);
}
