#include <Game/GameApp.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <gtest/gtest.h>

class GameTest : public testing::Test
{
protected:
	void SetUp() final
	{
		context = app.getContext();
		context->getDispatcher() << sw::io::CreateMap{10, 10};
	}

	void TearDown() final
	{
		app.clear();
	}

	sw::game::GameApp app;
	std::shared_ptr<sw::ecs::Context> context;
};
