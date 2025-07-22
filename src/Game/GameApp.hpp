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
		bool loadFromStream(std::istream& stream);
		bool loadFromFile(const std::string& fileName);
		bool loadFromStr(const std::string& scenario);

	private:
		void init();

		std::shared_ptr<ecs::Context> context;
	};
}
