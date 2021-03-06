#pragma once
#include "Name.hpp"

namespace oeng::ai_state
{
	class Base
	{
	public:
		explicit Base(class AIComponent& owner) noexcept: owner_{owner} {}
		explicit Base(class AIComponent* owner) noexcept: owner_{*owner} {}
		virtual ~Base() = default;
		virtual void OnEnter(Base& prev) {}
		virtual void Update(Float delta_seconds) {}
		virtual void OnExit(Base& next) {}
		[[nodiscard]] virtual Name GetName() const = 0;
		[[nodiscard]] AIComponent& GetOwner() const noexcept { return owner_; }

	private:
		AIComponent& owner_;
	};
}
