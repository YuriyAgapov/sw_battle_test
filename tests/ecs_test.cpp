#include <ECS/Context.hpp>
#include <ECS/System.hpp>
#include <gtest/gtest.h>

using namespace sw;

class TestCompA
{};

class TestCompB
{};

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
	context->addEntity(1);
	EXPECT_TRUE(context->hasEntity(1));
	EXPECT_TRUE(context->getEntities().contains(1));
}

TEST_F(EcsTest, RemoveEntity)
{
	context->addEntity(1);
	context->removeEntity(1);
	// still actual
	EXPECT_TRUE(context->hasEntity(1));

	// advance and check
	context->advance();
	EXPECT_FALSE(context->hasEntity(1));
}

TEST_F(EcsTest, AddComponent)
{
	context->addEntity(1);
	auto addedComp = context->addComponent<TestCompA>(1);

	for (auto& [_, comp] : context->getComponents())
	{
		EXPECT_EQ(addedComp, comp);
	}
}

TEST_F(EcsTest, getComponent)
{
	context->addEntity(1);
	auto addedComp = context->addComponent<TestCompA>(1);
	auto foundComp = context->getComponent<TestCompA>(1);
	EXPECT_EQ(addedComp, foundComp);
}

TEST_F(EcsTest, getComponents)
{
	context->addEntity(1);
	auto comp1 = context->addComponent<TestCompA>(1);
	auto comp2 = context->addComponent<TestCompB>(1);

	auto [foundA, foundB] = context->getComponents<TestCompA, TestCompB>(1);

	EXPECT_EQ(comp1, foundA);
	EXPECT_EQ(comp2, foundB);
}

TEST_F(EcsTest, forEachSimple)
{
	context->addEntity(1);
	auto comp = context->addComponent<TestCompA>(1);

	std::vector<std::shared_ptr<TestCompA>> comps;
	context->for_each<TestCompA>(
		[&comps](const uint32_t entityId, auto comp)
		{
			//...
			comps.push_back(comp);
			return true;
		});
	EXPECT_EQ(comps.size(), 1);
	EXPECT_EQ(comps[0], comp);
}

TEST_F(EcsTest, forEachBreak)
{
	context->addEntity<TestCompA>(1);
	context->addEntity<TestCompA>(2);
	context->addEntity<TestCompA>(3);

	uint32_t counter = 0;
	context->for_each<TestCompA>(
		[&counter](const uint32_t entityId, auto comp)
		{
			++counter;
			return counter != 2;
		});
	EXPECT_EQ(counter, 2);
}

TEST_F(EcsTest, forEachSeveral)
{
	context->addEntity<TestCompA>(1);
	context->addEntity<TestCompA, TestCompB>(2);
	context->addEntity<TestCompB>(3);

	uint32_t aCount = 0;
	context->for_each<TestCompA>(
		[&aCount](const uint32_t entityId, auto comp)
		{
			++aCount;
			return true;
		});
	uint32_t bCount = 0;
	context->for_each<TestCompB>(
		[&bCount](const uint32_t entityId, auto comp)
		{
			++bCount;
			return true;
		});
	uint32_t abCount = 0;
	context->for_each<TestCompA, TestCompB>(
		[&abCount](const uint32_t entityId, auto compA, auto compB)
		{
			++abCount;
			return true;
		});
	EXPECT_EQ(aCount, 2);
	EXPECT_EQ(bCount, 2);
	EXPECT_EQ(abCount, 1);
}

TEST_F(EcsTest, dispatchEvents)
{
	struct MyEvent
	{};

	uint32_t eventCount = 0;
	context->getDispatcher().subscribe<MyEvent>([&eventCount](const MyEvent& event) { ++eventCount; });

	// event was enqueued
	context->getDispatcher() << MyEvent{};
	EXPECT_EQ(eventCount, 0);

	// event dispatched
	context->advance();
	EXPECT_EQ(eventCount, 1);

	// enent queue is clear
	context->advance();
	EXPECT_EQ(eventCount, 1);
}

TEST_F(EcsTest, eventsOrder)
{
	ecs::EventDispatcher& dispatcher = context->getDispatcher();

	std::vector<int8_t> eventsOrder;
	dispatcher.subscribe<int8_t>([&eventsOrder](const auto&) { eventsOrder.emplace_back(1); });
	dispatcher.subscribe<int16_t>([&eventsOrder](const auto&) { eventsOrder.emplace_back(2); });
	dispatcher.subscribe<int32_t>([&eventsOrder](const auto&) { eventsOrder.emplace_back(3); });

	dispatcher << int8_t{} << int32_t{} << int16_t{};
	dispatcher.dispatchAll();
	const std::vector<int8_t> expectedOrder{1, 3, 2};
	EXPECT_EQ(eventsOrder, expectedOrder);
}

TEST_F(EcsTest, nestedEvents)
{
	ecs::EventDispatcher& dispatcher = context->getDispatcher();

	std::vector<int8_t> eventsOrder;
	dispatcher.subscribe<int8_t>(
		[&eventsOrder, &dispatcher](const auto&)
		{
			eventsOrder.emplace_back(1);
			dispatcher << int16_t{};
		});
	dispatcher.subscribe<int16_t>([&eventsOrder](const auto&) { eventsOrder.emplace_back(2); });
	dispatcher << int8_t{};
	dispatcher.dispatchAll();
	const std::vector<int8_t> expectedOrder{1, 2};
	EXPECT_EQ(eventsOrder, expectedOrder);
}

TEST_F(EcsTest, singletoneComponent)
{
	struct Singletone
	{
		uint32_t value = 0;
	};

	context->addSingletoneComponent<Singletone>(42);
	EXPECT_EQ(context->addSingletoneComponent<Singletone>(73), nullptr);

	EXPECT_TRUE(context->getEntities().empty());
	EXPECT_EQ(context->getComponents().size(), 1);

	EXPECT_EQ(context->getSingletoneComponent<Singletone>()->value, 42);
}
