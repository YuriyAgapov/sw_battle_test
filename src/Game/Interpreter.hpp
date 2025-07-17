#pragma once

#include "Scheme/Attributes.hpp"
#include "Scheme/Units.hpp"
#include <IO/Commands/SpawnSwordsman.hpp>

#include <functional>
#include <memory>
#include <cstdint>

namespace sw::game
{
	class Unit;



	class EventDispatcher
	{
	public:
		void enqueEvent()
		{
		}
	private:
	};

	struct AbilityContext
	{
		std::shared_ptr<Unit> Target;
	};

	class Ability
	{
	public:
		Ability(const std::shared_ptr<Unit>& Owner) : Owner(Owner)
		{
		}

		virtual bool CanActivate() const = 0;
		virtual void Activate(const AbilityContext& context) = 0;
		virtual float getPriority() const = 0;

	private:
		std::weak_ptr<Unit> Owner;
	};

	class AbilityRegistry
	{
	public:
		using CreateMethod = std::function<std::unique_ptr<Ability> (const std::weak_ptr<Unit>&)>;

		void add(const scheme::AbilityType type, CreateMethod method)
		{
			methods.emplace(type, std::move(method));
		}

		std::unique_ptr<Ability> create(const scheme::AbilityType type, const std::weak_ptr<Unit>& owner)
		{
			auto iter = methods.find(type);
			return iter == methods.end() ? nullptr : iter->second(owner);
		}


		static AbilityRegistry& get()
		{
			static AbilityRegistry abilityRegistry;
			return abilityRegistry;
		}

	private:
		std::unordered_map<scheme::AbilityType, CreateMethod> methods;
	};

	class MoveAbility : public Ability
	{
	};

	class Unit : public std::enable_shared_from_this<Unit>
	{
	public:
		Unit(const scheme::UnitType type, scheme::AttributeValues attributes)
				: type(type)
				, attributes(std::move(attributes))
		{
			if (!scheme::isValidAttributes(type, attributes))
			{
				throw std::runtime_error("Invalid attribute set for unit");
			}
		}

		void InitAbilities(scheme::AbilitySet abilities)
		{
			for (const scheme::AbilityType abilityType : abilities)
			{
				AbilityRegistry::get().create(abilityType, shared_from_this());
			}
		}
		uint32_t getId() const
		{
			return attributes.at(scheme::AttributeType::UnitId);
		}

		scheme::AttributeValues& getAttributes()
		{
			return attributes;
		}

		void Evaluate()
		{
		}
	private:
		scheme::UnitType type;
		std::vector<std::unique_ptr<Ability>> abilities;
		scheme::AttributeValues attributes;
	};

	class World
	{
	public:
		World(const uint32_t w, const uint32_t h) :
				w(w),
				h(h)
		{
		}
		bool isValid() const
		{
			return w > 0 && h > 0;
		}

		void addUnit(const std::shared_ptr<Unit>& unit)
		{
			if (!unit)
			{
				throw std::runtime_error("Failed to add invalid unit to the world");
			}

			units.emplace(unit->getId(), unit);
		}

	private:
		uint32_t w{};
		uint32_t h{};
		std::unordered_map<uint32_t, std::shared_ptr<Unit>> units;
	};


	class Interpreter
	{
	public:
		void AddUnit();
	};
};
