#include "TimerManager.h"
#include "World.h"

namespace Game
{
	struct CTimerManager::FTimer
	{
		std::function<Loop()> fn;
		TimePoint end;
		Duration delay;
	};

	FTimerHandle CTimerManager::SetLoopTimer(Duration delay, std::function<Loop()>&& fn)
	{
		const auto handle = FTimerHandle::Create();
		pending_timers_.insert({handle, {std::move(fn), world_.GetTime() + delay, delay}});
		return handle;
	}

	FTimerHandle CTimerManager::SetTimer(Duration delay, std::function<void()>&& fn)
	{
		return SetTimer(delay, [func = std::move(fn)]{func(); return Loop::kStop;});
	}

	void CTimerManager::SetTimerForNextTick(std::function<void()>&& fn)
	{
		SetTimer({}, std::move(fn));
	}

	FTimerHandle FTimerHandle::Create() noexcept
	{
		static auto key = 0;
		FTimerHandle handle;
		handle.key = key++;
		return handle;
	}

	CTimerManager::CTimerManager(CWorld& world):
		world_{world}
	{
	}

	CTimerManager::~CTimerManager() = default;

	void CTimerManager::Update()
	{
		timers_.merge(pending_timers_);
		
		const auto cur = world_.GetTime();
		for (auto it = timers_.begin(); it != timers_.end();)
		{
			auto& [handle, timer] = *it;
			if (timer.end <= cur)
			{
				const auto loop = timer.fn();
				if (loop == Loop::kContinue)
				{
					timer.end += timer.delay;
					++it;
				}
				else
				{
					it = timers_.erase(it);
				}
			}
			else
			{
				++it;
			}
		}
	}
}

size_t std::hash<Game::FTimerHandle>::operator()(const Game::FTimerHandle& key) const noexcept
{
	return size_t(key.key);
}