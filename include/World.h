#pragma once
#include <memory>
#include <vector>
#include "TimeUtil.h"

union SDL_Event;

namespace Game
{
	class CEngine;
	class CRenderer;
	class CTimerManager;
	class AActor;
	class CInputComponent;
	class CCircleComponent;

	class CWorld
	{
	public:
		explicit CWorld(CEngine& engine);
		
		template <class T>
		T& SpawnActor()
		{
			static_assert(std::is_base_of_v<AActor, T>);
			auto ptr = std::make_unique<T>(*this);
			auto& actor = *ptr;
			RegisterActor(std::move(ptr));
			return actor;
		}

		void Tick();

		void RegisterInputComponent(const CInputComponent& comp);
		void UnregisterInputComponent(const CInputComponent& comp);

		void RegisterCollision(CCircleComponent& comp);
		void UnregisterCollision(CCircleComponent& comp);
		
		[[nodiscard]] CEngine& GetEngine() const noexcept { return engine_; }
		[[nodiscard]] CRenderer& GetRenderer() const noexcept { return *renderer_; }
		[[nodiscard]] CTimerManager& GetTimerManager() const noexcept { return *timer_; }
		[[nodiscard]] auto GetTime() const noexcept { return time_; }

	private:
		void UpdateGame();
		float UpdateTime();
		void RegisterActor(std::unique_ptr<AActor>&& actor);

		CEngine& engine_;
		std::unique_ptr<CRenderer> renderer_;
		std::unique_ptr<CTimerManager> timer_;

		std::vector<std::reference_wrapper<CCircleComponent>> collisions_;
		
		std::vector<std::unique_ptr<AActor>> actors_;
		std::vector<std::unique_ptr<AActor>> pendingActors_;
		
		TimePoint time_;
	};
}
