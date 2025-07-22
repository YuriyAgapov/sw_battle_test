#pragma once

#include <ECS/Context.hpp>

namespace sw::game
{
	class GameApp
	{
	public:
		GameApp();

		const std::shared_ptr<ecs::Context>& getContext() const;

		void exec(const uint32_t tickLimit);
		void clear();
		bool loadFromFile(const std::string& fileName);

	private:
		void init();

		std::shared_ptr<ecs::Context> context;
	};
}
