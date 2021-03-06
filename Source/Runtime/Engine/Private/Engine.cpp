﻿#include "Engine.hpp"
#include <SDL.h>
#include "Log.hpp"

namespace oeng
{
	Engine::Engine(std::string_view game_name, const Function<void(Engine&)>& load_game)
		:game_name_{game_name}, renderer_{*this, {1024, 768}}, world_{*this}
	{
		log::Info("Engine initialization successful.");
		log::Info("Loading game module...");
		load_game(*this);
		log::Info("Game module loaded.");
	}

	Engine::~Engine() = default;

	void Engine::RunLoop()
	{
		log::Info("Engine loop started.");
		
		while (is_running_)
		{
			Tick();
		}
	}

	void Engine::Shutdown()
	{
		is_running_ = false;
		log::Info("Engine shutdown requested.");
	}

	Vec2u16 Engine::GetScreenSize() const noexcept
	{
		return renderer_.GetScreenSize();
	}

	void Engine::Tick()
	{
		ProcessEvent();
		world_.Tick();
		renderer_.DrawScene();
	}

	void Engine::ProcessEvent()
	{
		input_system_.ClearEvents();
		
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				Shutdown();
				return;
			}
			
			input_system_.AddEvent(event);
		}
	}

	extern OE_IMPORT bool engine_exist;

	SdlRaii::SdlRaii()
	{
		if (engine_exist) throw std::runtime_error{"Only 1 engine instance can exists"};
		engine_exist = true;

		if (!IsGameThread()) throw std::runtime_error{"The engine instance must be created on the main thread"};
		
		log::Info("Initializing engine...");
		omem::SetOnPoolDest([](auto&&...){});
		
		const auto sdl_result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
		if (sdl_result != 0) throw std::runtime_error{SDL_GetError()};
	}

	SdlRaii::~SdlRaii()
	{
		SDL_Quit();

		for (auto&& [size, pool] : omem::GetPools())
		{
			const auto& info = pool.GetInfo();
			log::Info("[Mem] Memory pool with {} {}-byte blocks", info.count, info.size);
			log::Info("[Mem]  Peak usage: {} blocks", info.peak);
			if (info.fault) log::Info("[Mem]  Block fault: {} times", info.fault);
			if (info.cur) log::Warn("[Mem]  Leaked: {} blocks", info.cur);
		}
	}
}
