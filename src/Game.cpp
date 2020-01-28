﻿#include <Game.h>
#include <thread>
#include <stdexcept>
#include <SDL.h>
#include <SDL_image.h>
#include <MathUtils.h>
#include <Actors/Actor.h>
#include <Components/SpriteComponent.h>

NEG_BEGIN

static constexpr auto screen_w = 1024;
static constexpr auto screen_h = 768;

static SDL_Window* create_window()
{	
	const auto window = SDL_CreateWindow(PROJECT_NAME, 100, 100, screen_w, screen_h, 0);
	if (!window) throw std::runtime_error{SDL_GetError()};
	return window;
}

static SDL_Renderer* create_renderer(SDL_Window* const window)
{
	const auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) throw std::runtime_error{SDL_GetError()};
	return renderer;
}

game::game()
	:is_running_{true}, is_updating_actors_{},
	window_{create_window(), SDL_DestroyWindow},
	renderer_{create_renderer(window_.get()), SDL_DestroyRenderer}
{
}

game::~game() = default;

void game::run_loop()
{
	while (is_running_)
	{
		process_input();
		update_game();
		generate_output();
	}
}

void game::shutdown()
{
	is_running_ = false;
}

void game::add_actor(actor_ptr&& actor)
{
	if (is_updating_actors_)
	{
		pending_actors_.push_back(std::move(actor));
	}
	else
	{
		actors_.push_back(std::move(actor));
	}
}

std::shared_ptr<SDL_Texture> game::load_texture(const char* const filename)
{
	const std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> surface{IMG_Load(filename), SDL_FreeSurface};
	if (!surface) throw std::runtime_error{SDL_GetError()};

	auto deleter = [this, filename](SDL_Texture* const texture)
	{
		textures_.erase(filename);
		SDL_DestroyTexture(texture);
	};
	std::shared_ptr<SDL_Texture> texture{SDL_CreateTextureFromSurface(renderer_.get(), surface.get()), std::move(deleter)};
	if (!texture) throw std::runtime_error{SDL_GetError()};

	return texture;
}

std::shared_ptr<SDL_Texture> game::get_texture(const char* const filename)
{
	const auto found = textures_.find(filename);
	if (found != textures_.end()) return found->second.lock();

	const auto loaded = load_texture(filename);
	textures_.emplace(filename, loaded);

	return loaded;
}

void game::add_sprite(const sprite_component& sprite)
{
	auto cmp = [](const sprite_component& a, const sprite_component& b)
	{
		return a.get_draw_order() <= b.get_draw_order();
	};
	const auto pos = std::lower_bound(sprites_.begin(), sprites_.end(), sprite, cmp);
	sprites_.emplace(pos, sprite);
}

void game::remove_sprite(const sprite_component& sprite)
{
	auto pr = [&](const sprite_component& v) { return &v == &sprite; };
	const auto found = std::find_if(sprites_.crbegin(), sprites_.crend(), pr);
	if (found != sprites_.crend()) sprites_.erase(found.base() - 1);
}

void game::process_input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			shutdown();
			break;
		default:
			break;
		}
	}

	const auto keyboard = SDL_GetKeyboardState(nullptr);
	if (keyboard[SDL_SCANCODE_ESCAPE]) shutdown();
}

void game::update_game()
{
	constexpr auto max_fps = 60, min_fps = 10;
	constexpr auto time_scale = 1.f;
	
	std::this_thread::sleep_for(std::chrono::milliseconds{ticks_count_ + 1000ll/max_fps - SDL_GetTicks()});
	
	const auto delta_time = math::min((SDL_GetTicks() - ticks_count_) / 1000.f, 1.f/min_fps) * time_scale;
	ticks_count_ = SDL_GetTicks();

	is_updating_actors_ = true;
	for (const auto& actor : actors_)
	{
		actor->update(delta_time);
	}
	is_updating_actors_ = false;

	for (auto&& pending : pending_actors_)
	{
		actors_.push_back(std::move(pending));
	}
	pending_actors_.clear();

	for (auto it = actors_.rbegin(); it != actors_.rend();)
	{
		const auto& actor = **it;
		if (actor.get_state() == actor::state::dead)
		{
			const auto next = actors_.erase(it.base() - 1);
			it = std::make_reverse_iterator(next);
		}
		else
		{
			++it;
		}
	}
}

void game::generate_output() const
{
	SDL_SetRenderDrawColor(renderer_.get(), 100, 100, 250, 255);
	SDL_RenderClear(renderer_.get());
	
	SDL_RenderPresent(renderer_.get());
}

game::sdl_raii::sdl_raii()
{
	const auto sdl_result = SDL_Init(SDL_INIT_VIDEO);
	if (sdl_result != 0) throw std::runtime_error{SDL_GetError()};

	const auto flags = IMG_INIT_PNG;
	const auto img_result = IMG_Init(flags);
	if (img_result != flags) throw std::runtime_error{IMG_GetError()};
}

game::sdl_raii::~sdl_raii()
{
	IMG_Quit();
	SDL_Quit();
}

NEG_END
