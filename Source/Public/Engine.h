﻿#pragma once
#include <memory>
#include <unordered_map>
#include "Name.h"
#include "Math.hpp"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;

namespace oeng
{
	class Renderer;
	class Texture;
	class CWorld;
	class CInputSystem;

	class CSdlRaii
	{
	public:
		CSdlRaii();
		~CSdlRaii();
	};
	
	class CEngine : CSdlRaii
	{
	public:
		CEngine();
		~CEngine();
		
		void RunLoop();
		void Shutdown();
		
		[[nodiscard]] std::shared_ptr<Texture> GetTexture(Name file);
		[[nodiscard]] CWorld& GetWorld() const noexcept { return *world_; }
		[[nodiscard]] CInputSystem& GetInputSystem() const noexcept { return *input_system_; }
		[[nodiscard]] Renderer& GetRenderer() const noexcept { return *renderer_; }
		[[nodiscard]] Vec2u16 GetScreenSize() const noexcept;
		
	private:
		void Tick();
		void ProcessEvent();

		std::unordered_map<Name, std::weak_ptr<Texture>> textures_;
		std::unique_ptr<Renderer> renderer_;
		std::unique_ptr<CWorld> world_;
		std::unique_ptr<CInputSystem> input_system_;
		bool is_running_ = true;
	};
}
