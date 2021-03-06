﻿#pragma once
#include "Interfaces/Engine.hpp"
#include "InputSystem.hpp"
#include "Math.hpp"
#include "World.hpp"
#include "Renderer.hpp"
#include "Templates/Function.hpp"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;

namespace oeng
{
	class Texture;
	class Mesh;

	class SdlRaii
	{
	public:
		SdlRaii();
		~SdlRaii();
	};
	
	class OEAPI Engine : public IEngine, SdlRaii
	{
	public:
		Engine(std::string_view game_name, const Function<void(Engine&)>& load_game);
		~Engine();
		
		void RunLoop();
		void Shutdown();
		
		[[nodiscard]] World& GetWorld() noexcept override { return world_; }
		[[nodiscard]] InputSystem& GetInputSystem() noexcept override { return input_system_; }
		[[nodiscard]] Renderer& GetRenderer() noexcept override { return renderer_; }
		[[nodiscard]] Vec2u16 GetScreenSize() const noexcept override;
		[[nodiscard]] std::string_view GetGameName() const noexcept override { return game_name_; }

		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;
		
	private:
		void Tick();
		void ProcessEvent();

		std::string_view game_name_;
		Renderer renderer_;
		World world_;
		InputSystem input_system_;
		bool is_running_ = true;
	};
}
